#include "AVCodecHelper.h"

#include <cstring>

#include <QVector>

#include "AVFormatHelper.h"

namespace avcodec {
	EncoderParameters::EncoderParameters()
	: iWidth(0)
	, iHeight(0)
	, timeBase({ 0, 0 })
	, pixelFormat(AV_PIX_FMT_NONE)
	, colorRange(AVCOL_RANGE_UNSPECIFIED)
	, iCRF(0)
	{

	}

	Context::Context()
	: m_pContext(nullptr)
	, m_pCodec(nullptr)
	, m_pPacket(nullptr)
	, m_pFrame(nullptr)
	{

	}

	Context::~Context()
	{
		if (m_pContext != nullptr) {
			avcodec_free_context(&m_pContext);
		}

		if (m_pPacket != nullptr) {
			av_packet_free(&m_pPacket);
		}

		if (m_pFrame != nullptr) {
			av_frame_free(&m_pFrame);
		}
	}

	EncoderParameters Context::getCodecParameters() const
	{
		EncoderParameters parameters;

		if (m_pContext == nullptr) {
			return parameters;
		}

		parameters.iWidth = m_pContext->width;
		parameters.iHeight = m_pContext->height;
		setPixelFormantAndColorRange(parameters);
		parameters.timeBase = m_pContext->time_base;

		return parameters;
	}

	const AVCodec* Context::getCodec() const
	{
		if (m_pContext == nullptr) {
			return nullptr;
		}

		return m_pContext->codec;
	}

	void Context::setCodec(const AVCodec* pCodec)
	{
		m_pCodec = pCodec;
	}

	Error Context::decodeVideoFile(const char* szVideoFileName, QVector<QByteArray>& yuvFrames)
	{
		avformat::Context formatContext;

		if (formatContext.openFile(szVideoFileName, *this) != avformat::Error::Success) {
			return Error::OpenCodec;
		}

		Error error = openDecoder(formatContext);
		if (error != avcodec::Error::Success) {
			return Error::OpenCodec;
		}

		avcodec::Error errorCodec = avcodec::Error::Success;
		do {
			errorCodec = decodePacket(formatContext, yuvFrames);
		} while (errorCodec == avcodec::Error::Success);

		if (errorCodec != avcodec::Error::CodecFlushed) {
			return Error::Unkown;
		}

		return Error::Success;
	}

	Error Context::decodePacketStream(QVector<QByteArray>& packets, const AVCodec* pCodec, QVector<QByteArray>& yuvFrames)
	{
		Error error = Error::Success;

		if (openDecoder(pCodec) != avcodec::Error::Success) {
			return Error::OpenCodec;
		}

		for (auto& packet: packets) {
			m_pPacket->data = reinterpret_cast<uint8_t*>(packet.data());
			m_pPacket->size = packet.size();

			if (decodeVideoFrame(m_pPacket, yuvFrames) != avcodec::Error::Success) {
				return Error::Unkown;
			}
		}

		// Flush decoder
		if (decodeVideoFrame(nullptr, yuvFrames) != avcodec::Error::CodecFlushed) {
			return Error::Unkown;
		}

		return error;
	}

	Error Context::encodeFrameStream(const QVector<QByteArray>& yuvFrames, const EncoderParameters& parameters, QVector<QByteArray>& packets)
	{
		if (openEncoder("libx265", parameters) != avcodec::Error::Success) {
			return Error::OpenCodec;
		}

		for (const auto& yuvFrame: yuvFrames) {
			if (encodeFrame(yuvFrame, packets) != avcodec::Error::Success) {
				return Error::Unkown;
			}
		}

		// Flush decoder
		if (encodeFrame(QByteArray(), packets) != avcodec::Error::CodecFlushed) {
			return Error::Unkown;
		}

		return Error::Success;
	}

	Error Context::allocateContext()
	{
		m_pPacket = av_packet_alloc();
		if (m_pPacket == nullptr) {
			return Error::NoMemory;
		}

		m_pFrame = av_frame_alloc();
		if (m_pFrame == nullptr) {
			return Error::NoMemory;
		}

		m_pContext = avcodec_alloc_context3(m_pCodec);
		if (m_pContext == nullptr) {
			return Error::NoMemory;
		}

		return Error::Success;
	}

	void Context::setPixelFormantAndColorRange(EncoderParameters& parameters) const
	{
		switch (m_pContext->pix_fmt) {
		case AV_PIX_FMT_YUVJ420P:
			parameters.pixelFormat = AV_PIX_FMT_YUV420P;
			parameters.colorRange = AVCOL_RANGE_JPEG;
			break;

		case AV_PIX_FMT_YUVJ422P:
			parameters.pixelFormat = AV_PIX_FMT_YUV422P;
			parameters.colorRange = AVCOL_RANGE_JPEG;
			break;

		case AV_PIX_FMT_YUVJ444P:
			parameters.pixelFormat = AV_PIX_FMT_YUV444P;
			parameters.colorRange = AVCOL_RANGE_JPEG;
			break;

		default:
			parameters.pixelFormat = m_pContext->pix_fmt;
			parameters.colorRange = m_pContext->color_range;
		}
	}

	Error Context::openDecoder(const avformat::Context& formatContext)
	{
		if (allocateContext() != Error::Success) {
			return Error::NoMemory;
		}

		auto videoStream = formatContext.getVideoStream();
		if (avcodec_parameters_to_context(m_pContext, videoStream.pStream->codecpar) < 0) {
			return Error::CopyParameters;
		}

		if (avcodec_open2(m_pContext, m_pCodec, nullptr) < 0) {
			return Error::OpenCodec;
		}

		return Error::Success;
	}

	Error Context::openDecoder(const AVCodec* pCodec)
	{
		if (allocateContext() != Error::Success) {
			return Error::NoMemory;
		}

		AVCodec* pTmpCodec = nullptr;

		// TODO: Create enum to handle this
		if (QString(pCodec->name) == "libx265") {
			pTmpCodec = avcodec_find_decoder_by_name("hevc");
		}

		if (pTmpCodec == nullptr) {
			return Error::NoCodecFound;
		}
		m_pCodec = pTmpCodec;

		if (avcodec_open2(m_pContext, m_pCodec, nullptr) < 0) {
			return Error::OpenCodec;
		}

		return Error::Success;
	}

	Error Context::decodeVideoFrame(const AVPacket* pPacket, QVector<QByteArray>& yuvFrames)
	{
		if (avcodec_send_packet(m_pContext, pPacket) < 0) {
			return Error::SendPacket;
		}

		int iRes = 0;
		while (iRes >= 0) {
			iRes = avcodec_receive_frame(m_pContext, m_pFrame);
			if (iRes == AVERROR(EAGAIN)) {
				return Error::Success;
			} else if (iRes == AVERROR_EOF) {
				return Error::CodecFlushed;
			} else if (iRes < 0) {
				return Error::ReceiveFrame;
			}

			// Dump yuv
			QByteArray yuvBytes;
			yuvBytes.append(reinterpret_cast<const char*>(m_pFrame->data[0]), m_pFrame->width * m_pFrame->height);
			yuvBytes.append(reinterpret_cast<const char*>(m_pFrame->data[1]), m_pFrame->width * m_pFrame->height / 4);
			yuvBytes.append(reinterpret_cast<const char*>(m_pFrame->data[2]), m_pFrame->width * m_pFrame->height / 4);

			yuvFrames.append(yuvBytes);
		}

		return Error::Success;
	}

	Error Context::decodePacket(avformat::Context& formatContext, QVector<QByteArray>& yuvFrames)
	{
		Error codecError = Error::Success;
		auto formatError = formatContext.readVideoFrame(*m_pPacket);

		if (formatError == avformat::Error::Success) {
			codecError = decodeVideoFrame(m_pPacket, yuvFrames);
		} else if (formatError == avformat::Error::EndOfFile) {
			codecError = decodeVideoFrame(nullptr, yuvFrames);
		} else {
			codecError = Error::Unkown;
		}

		av_packet_unref(m_pPacket);

		return codecError;
	}

	Error Context::openEncoder(const char* szCodecName, const EncoderParameters& parameters)
	{
		const AVCodec* pCodec = avcodec_find_encoder_by_name(szCodecName);
		if (pCodec == nullptr) {
			return Error::NoCodecFound;
		}
		m_pCodec = pCodec;

		if (allocateContext() != Error::Success) {
			return Error::NoMemory;
		}

		m_pContext->time_base = parameters.timeBase;
		m_pContext->framerate = { parameters.timeBase.num, parameters.timeBase.den };
		m_pContext->pix_fmt = parameters.pixelFormat;
		m_pContext->color_range = parameters.colorRange;
		m_pContext->width = parameters.iWidth;
		m_pContext->height = parameters.iHeight;
		m_pContext->thread_count = 1;

		AVDictionary* options = nullptr;
		av_dict_set(&options, "crf", std::to_string(parameters.iCRF).c_str(), 0);
		av_dict_set(&options, "preset", qPrintable(parameters.szPreset), 0);
		av_dict_set(&options, "frame-threads", "1", 0);

		if (avcodec_open2(m_pContext, m_pCodec, &options) < 0) {
			return Error::OpenCodec;
		}

		m_pFrame->format = parameters.pixelFormat;
		m_pFrame->color_range = parameters.colorRange;
		m_pFrame->width = parameters.iWidth;
		m_pFrame->height = parameters.iHeight;

		if (av_frame_get_buffer(m_pFrame, 0) < 0) {
			return Error::NoMemory;
		}

		assert(m_pContext->thread_count == 1);

		return Error::Success;
	}

	Error Context::encodeVideoFrame(const AVFrame* pFrame, QVector<QByteArray>& packets)
	{
		if (avcodec_send_frame(m_pContext, pFrame) < 0) {
			return Error::SendFrame;
		}

		int iRes = 0;
		while (iRes >= 0) {
			iRes = avcodec_receive_packet(m_pContext, m_pPacket);
			if (iRes == AVERROR(EAGAIN)) {
				return Error::Success;
			} else if (iRes == AVERROR_EOF) {
				return Error::CodecFlushed;
			} else if (iRes < 0) {
				return Error::ReceiveFrame;
			}

			// Dump yuv
			QByteArray packetBytes;
			packetBytes.append(reinterpret_cast<const char*>(m_pPacket->data), m_pPacket->size);

			packets.append(packetBytes);

			av_packet_unref(m_pPacket);
		}

		return Error::Success;
	}

	Error Context::encodeFrame(const QByteArray& yuvFrame, QVector<QByteArray>& packets)
	{
		if (yuvFrame.isEmpty()) {
			return encodeVideoFrame(nullptr, packets);
		}

		// Fill AVFrame
		const char* pSrc = yuvFrame.data();
		std::memcpy(m_pFrame->data[0], pSrc, m_pFrame->width * m_pFrame->height);
		pSrc += m_pFrame->width * m_pFrame->height;
		std::memcpy(m_pFrame->data[1], pSrc, m_pFrame->width * m_pFrame->height / 4);
		pSrc += m_pFrame->width * m_pFrame->height / 4;
		std::memcpy(m_pFrame->data[2], pSrc, m_pFrame->width * m_pFrame->height / 4);

		return encodeVideoFrame(m_pFrame, packets);
	}
}

#include "AVCodecHelper.h"

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}

#include <cassert>
#include <cstring>

#include <QVector>

#include "AVFormatHelper.h"

namespace helper {
	namespace avcodec {
		namespace details {
			PixelFormat convertPixelFormat(AVPixelFormat pixelFormat)
			{
				switch (pixelFormat) {
				// case AV_PIX_FMT_GRAY8:
				// case AV_PIX_FMT_GRAY9:
				// case AV_PIX_FMT_GRAY10:
				// case AV_PIX_FMT_GRAY12:
				// case AV_PIX_FMT_GRAY14:
				// case AV_PIX_FMT_GRAY16:

				case AV_PIX_FMT_YUV420P:
				case AV_PIX_FMT_YUVJ420P:
				case AV_PIX_FMT_YUV420P9:
				case AV_PIX_FMT_YUV420P10:
				case AV_PIX_FMT_YUV420P12:
				case AV_PIX_FMT_YUV420P14:
				case AV_PIX_FMT_YUV420P16:
					return PixelFormat::YUV420P;

				// case AV_PIX_FMT_YUV422P:
				// case AV_PIX_FMT_YUV422P9:
				// case AV_PIX_FMT_YUV422P10:
				// case AV_PIX_FMT_YUV422P12:
				// case AV_PIX_FMT_YUV422P14:
				// case AV_PIX_FMT_YUV422P16:


				// case AV_PIX_FMT_YUV444P:
				// case AV_PIX_FMT_YUV444P9:
				// case AV_PIX_FMT_YUV444P10:
				// case AV_PIX_FMT_YUV444P12:
				// case AV_PIX_FMT_YUV444P14:
				// case AV_PIX_FMT_YUV444P16:

				default:
					return PixelFormat::Undefined;
				}

				return PixelFormat::Undefined;
			}

			AVPixelFormat convertPixelFormat(PixelFormat pixelFormat)
			{
				switch (pixelFormat) {
				case PixelFormat::YUV420P:
					return AV_PIX_FMT_YUV420P;

				case PixelFormat::Undefined:
					return AV_PIX_FMT_NONE;
				}

				return AV_PIX_FMT_NONE;
			}

			ColorRange convertColorRange(AVColorRange colorRange)
			{
				switch (colorRange) {
				case AVCOL_RANGE_MPEG:
					return ColorRange::MPEG;

				case AVCOL_RANGE_JPEG:
					return ColorRange::JPEG;

				default:
					return ColorRange::Undefined;
				}

				return ColorRange::Undefined;
			}

			AVColorRange convertColorRange(ColorRange colorRange)
			{
				switch (colorRange) {
				case ColorRange::MPEG:
					return AVCOL_RANGE_MPEG;

				case ColorRange::JPEG:
					return AVCOL_RANGE_JPEG;

				case ColorRange::Undefined:
					return AVCOL_RANGE_UNSPECIFIED;
				}

				return AVCOL_RANGE_UNSPECIFIED;
			}

			int getPixelDepth(AVPixelFormat pixelFormat)
			{
				switch (pixelFormat) {
				case AV_PIX_FMT_GRAY8:
				case AV_PIX_FMT_YUV420P:
				case AV_PIX_FMT_YUVJ420P:
				case AV_PIX_FMT_YUV422P:
				case AV_PIX_FMT_YUV444P:
					return 8;

				// case AV_PIX_FMT_GRAY9:
				// case AV_PIX_FMT_YUV420P9:
				// case AV_PIX_FMT_YUV422P9:
				// case AV_PIX_FMT_YUV444P9:
				// 	return 9;

				// case AV_PIX_FMT_GRAY10:
				// case AV_PIX_FMT_YUV420P10:
				// case AV_PIX_FMT_YUV422P10:
				// case AV_PIX_FMT_YUV444P10:
				// 	return 10;

				// case AV_PIX_FMT_GRAY12:
				// case AV_PIX_FMT_YUV420P12:
				// case AV_PIX_FMT_YUV422P12:
				// case AV_PIX_FMT_YUV444P12:
				// 	return 12;

				// case AV_PIX_FMT_GRAY14:
				// case AV_PIX_FMT_YUV420P14:
				// case AV_PIX_FMT_YUV444P14:
				// case AV_PIX_FMT_YUV422P14:
				// 	return 14;

				// case AV_PIX_FMT_GRAY16:
				// case AV_PIX_FMT_YUV420P16:
				// case AV_PIX_FMT_YUV422P16:
				// case AV_PIX_FMT_YUV444P16:
				// 	return 16;

				default:
					return -1;
				}

				return -1;
			}

			AVCodecID getCodecID(CodecType codec)
			{
				switch (codec) {
				case CodecType::H265:
					return AV_CODEC_ID_HEVC;
				}

				return AV_CODEC_ID_NONE;
			}
		}

		Context::Context()
		: m_pContext(nullptr)
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

		CodecParameters Context::getCodecParameters() const
		{
			CodecParameters parameters;

			parameters.videoSize = { m_pContext->width, m_pContext->height };
			parameters.pixelFormat = details::convertPixelFormat(m_pContext->pix_fmt);
			parameters.colorRange = details::convertColorRange(m_pContext->color_range);
			parameters.iPixelDepth = details::getPixelDepth(m_pContext->pix_fmt);

			parameters.iFPS = m_pContext->time_base.den;

			return parameters;
		}

		Error Context::decodeVideoFile(const char* szVideoFileName, QVector<QByteArray>& yuvFrames)
		{
			avformat::Context formatContext;

			if (formatContext.openFile(szVideoFileName) != avformat::Error::Success) {
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

		Error Context::decodePacketStream(QVector<QByteArray>& packets, CodecType codecType, QVector<QByteArray>& yuvFrames)
		{
			Error error = Error::Success;

			if (openDecoder(codecType) != avcodec::Error::Success) {
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

		Error Context::encodeFrameStream(
			const QVector<QByteArray>& yuvFrames,
			const CodecParameters& parameters,
			const EncoderParameters& encoderParameters,
			QVector<QByteArray>& packets
		)
		{
			if (openEncoder(parameters, encoderParameters) != avcodec::Error::Success) {
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

		Error Context::allocateContext(const AVCodec* pCodec)
		{
			m_pPacket = av_packet_alloc();
			if (m_pPacket == nullptr) {
				return Error::NoMemory;
			}

			m_pFrame = av_frame_alloc();
			if (m_pFrame == nullptr) {
				return Error::NoMemory;
			}

			m_pContext = avcodec_alloc_context3(pCodec);
			if (m_pContext == nullptr) {
				return Error::NoMemory;
			}

			return Error::Success;
		}

		Error Context::openDecoder(const avformat::Context& formatContext)
		{
			const AVCodec* pCodec = formatContext.getCodec();

			if (allocateContext(pCodec) != Error::Success) {
				return Error::NoMemory;
			}

			auto videoStream = formatContext.getVideoStream();
			if (avcodec_parameters_to_context(m_pContext, videoStream.pStream->codecpar) < 0) {
				return Error::CopyParameters;
			}

			if (avcodec_open2(m_pContext, pCodec, nullptr) < 0) {
				return Error::OpenCodec;
			}

			return Error::Success;
		}

		Error Context::openDecoder(CodecType codecType)
		{
			AVCodec* pCodec = avcodec_find_decoder(details::getCodecID(codecType));

			if (allocateContext(pCodec) != Error::Success) {
				return Error::NoMemory;
			}

			if (pCodec == nullptr) {
				return Error::NoCodecFound;
			}

			if (avcodec_open2(m_pContext, pCodec, nullptr) < 0) {
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

		Error Context::openEncoder(const CodecParameters& parameters, const EncoderParameters& encoderParameters)
		{
			// Only support 8-bits
			assert(parameters.iPixelDepth == 8);

			const AVCodec* pCodec = avcodec_find_encoder(details::getCodecID(encoderParameters.codecType));
			if (pCodec == nullptr) {
				return Error::NoCodecFound;
			}

			if (allocateContext(pCodec) != Error::Success) {
				return Error::NoMemory;
			}

			m_pContext->time_base = { 1, parameters.iFPS };
			m_pContext->framerate = { parameters.iFPS, 1 };

			m_pContext->pix_fmt = details::convertPixelFormat(parameters.pixelFormat);
			m_pContext->color_range = details::convertColorRange(parameters.colorRange);

			m_pContext->width = parameters.videoSize.width;
			m_pContext->height = parameters.videoSize.height;

			m_pContext->thread_count = 1;

			AVDictionary* options = nullptr;
			av_dict_set(&options, "crf", std::to_string(encoderParameters.iCRF).c_str(), 0);
			av_dict_set(&options, "preset", qPrintable(encoderParameters.szPreset), 0);
			av_dict_set(&options, "frame-threads", "1", 0);

			if (avcodec_open2(m_pContext, pCodec, &options) < 0) {
				return Error::OpenCodec;
			}

			m_pFrame->format = m_pContext->pix_fmt;
			m_pFrame->color_range = m_pContext->color_range;
			m_pFrame->width = parameters.videoSize.width;
			m_pFrame->height = parameters.videoSize.height;

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
}

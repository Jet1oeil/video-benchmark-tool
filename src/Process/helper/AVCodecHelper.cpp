#include "AVCodecHelper.h"

#include <QVector>

#include "AVFormatHelper.h"

namespace avcodec {
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

	void Context::setCodec(AVCodec* pCodec)
	{
		m_pCodec = pCodec;
	}

	Error Context::open(const avformat::Context& formatContext)
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

		auto videoStream = formatContext.getVideoStream();
		if (avcodec_parameters_to_context(m_pContext, videoStream.pStream->codecpar) < 0) {
			return Error::CopyParameters;
		}

		if (avcodec_open2(m_pContext, m_pCodec, nullptr) < 0) {
			return Error::OpenCodec;
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
				return Error::EndOfFile;
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
}

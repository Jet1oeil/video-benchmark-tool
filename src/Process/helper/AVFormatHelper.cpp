#include "AVFormatHelper.h"

#include "AVCodecHelper.h"

namespace avformat {
	Context::Context()
	: m_pContext(nullptr)
	{

	}

	Context::~Context()
	{
		if (m_pContext != nullptr) {
			avformat_close_input(&m_pContext);
		}
	}

	const Stream& Context::getVideoStream() const
	{
		return m_videoStream;
	}

	Error Context::openFile(const char* szVideoFileName, avcodec::Context& codecContext)
	{
		if (avformat_open_input(&m_pContext, szVideoFileName, nullptr, nullptr) != 0) {
			return Error::InvalidInputFile;
		}

		if (avformat_find_stream_info(m_pContext, nullptr) < 0) {
			return Error::NoStreamInfo;
		}

		AVCodec* pCodec = nullptr;
		int iRes = av_find_best_stream(m_pContext, AVMEDIA_TYPE_VIDEO, -1, -1, &pCodec, 0);
		if (iRes < 0) {
			return Error::NoVideoStream;
		}
		codecContext.setCodec(pCodec);
		m_videoStream.pStream = m_pContext->streams[iRes];
		m_videoStream.iIndex = iRes;

		return Error::Success;
	}
}

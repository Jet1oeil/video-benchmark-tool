#include "AVFormatHelper.h"

#include "AVCodecHelper.h"

namespace avformat {
	Stream::Stream()
	: m_pStream(nullptr)
	{

	}

	Error Stream::findVideoStream(Context& context, avcodec::Codec& codec)
	{
		int iRes = av_find_best_stream(context.m_pContext, AVMEDIA_TYPE_VIDEO, -1, -1, &codec.m_pCodec, 0);
		if (iRes < 0) {
			return Error::NoVideoStream;
		}
		m_pStream = context.m_pContext->streams[iRes];

		return Error::Success;
	}

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

	Error Context::openFile(const char* szVideoFileName, Stream& stream, avcodec::Codec& codec)
	{
		if (avformat_open_input(&m_pContext, szVideoFileName, nullptr, nullptr) != 0) {
			return Error::InvalidInputFile;
		}

		if (avformat_find_stream_info(m_pContext, nullptr) < 0) {
			return Error::NoStreamInfo;
		}

		Error error = stream.findVideoStream(*this, codec);
		if (error != Error::Success) {
			return error;
		}

		return Error::Success;
	}
}

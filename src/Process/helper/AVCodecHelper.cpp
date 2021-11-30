#include "AVCodecHelper.h"

#include "AVFormatHelper.h"

namespace avcodec {
	Codec::Codec()
	: m_pCodec(nullptr)
	{

	}

	Context::Context()
	: m_pContext(nullptr)
	{

	}

	Context::~Context()
	{
		if (m_pContext != nullptr) {
			avcodec_free_context(&m_pContext);
		}
	}

	Error Context::open(const Codec& codec, const avformat::Stream& videoStream)
	{
		m_pContext = avcodec_alloc_context3(codec.m_pCodec);
		if (m_pContext == nullptr) {
			return Error::NoMemory;
		}

		if (avcodec_parameters_to_context(m_pContext, videoStream.m_pStream->codecpar) < 0) {
			return Error::CopyParameters;
		}

		if (avcodec_open2(m_pContext, codec.m_pCodec, nullptr) < 0) {
			return Error::OpenCodec;
		}

		return Error::Success;
	}
}

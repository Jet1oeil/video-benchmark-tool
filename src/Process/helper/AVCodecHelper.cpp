#include "AVCodecHelper.h"

#include "AVFormatHelper.h"

namespace avcodec {
	Context::Context()
	: m_pContext(nullptr)
	, m_pCodec(nullptr)
	{

	}

	Context::~Context()
	{
		if (m_pContext != nullptr) {
			avcodec_free_context(&m_pContext);
		}
	}

	void Context::setCodec(AVCodec* pCodec)
	{
		m_pCodec = pCodec;
	}

	Error Context::open(const avformat::Context& formatContext)
	{
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
}

#ifndef HELPER_AVCODEC_HELPER_H_
#define HELPER_AVCODEC_HELPER_H_

extern "C" {
	#include <libavcodec/avcodec.h>
}

namespace avformat {
	class Context;
}

namespace avcodec {
	enum class Error {
		Success,
		NoMemory,
		CopyParameters,
		OpenCodec,
		Unkown,
	};

	class Context {
	public:
		Context();
		~Context();

		Context(const Context&) = delete;
		Context(Context&&) = delete;

		Context& operator=(const Context&) = delete;
		Context& operator=(Context&&) = delete;

		void setCodec(AVCodec* pCodec);

		Error open(const avformat::Context& formatContext);

	private:
		AVCodecContext* m_pContext;
		AVCodec* m_pCodec;
	};
}

#endif // HELPER_AVCODEC_HELPER_H_

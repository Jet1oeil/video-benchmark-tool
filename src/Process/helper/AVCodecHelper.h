#ifndef HELPER_AVCODEC_HELPER_H_
#define HELPER_AVCODEC_HELPER_H_

extern "C" {
	#include <libavcodec/avcodec.h>
}

namespace avformat {
	class Stream;
}

namespace avcodec {
	enum class Error {
		Success,
		NoMemory,
		CopyParameters,
		OpenCodec,
		Unkown,
	};

	class Codec {
	public:
		Codec();
		~Codec() = default;

		Codec(const Codec&) = delete;
		Codec(Codec&&) = delete;

		Codec& operator=(const Codec&) = delete;
		Codec& operator=(Codec&&) = delete;

		friend class avformat::Stream;
		friend class Context;

	private:
		AVCodec* m_pCodec;
	};

	class Context {
	public:
		Context();
		~Context();

		Context(const Context&) = delete;
		Context(Context&&) = delete;

		Context& operator=(const Context&) = delete;
		Context& operator=(Context&&) = delete;

		Error open(const Codec& codec, const avformat::Stream& videoStream);

	private:
		AVCodecContext* m_pContext;
	};
}

#endif // HELPER_AVCODEC_HELPER_H_

#ifndef HELPER_AVFORMAT_HELPER_H_
#define HELPER_AVFORMAT_HELPER_H_

extern "C" {
	#include <libavformat/avformat.h>
}

namespace avcodec {
	class Codec;
	class Context;
}

namespace avformat {
	enum class Error {
		Success,
		InvalidInputFile,
		NoStreamInfo,
		NoVideoStream,
		Unkown,
	};

	class Context;

	class Stream {
	public:
		Stream();
		~Stream() = default;

		Stream(const Stream&) = delete;
		Stream(Stream&&) = delete;

		Stream& operator=(const Stream&);
		Stream& operator=(Stream&&);

		friend class Context;
		friend class avcodec::Context;

	private:
		Error findVideoStream(Context& context, avcodec::Codec& codec);

	private:
		AVStream* m_pStream;
	};

	class Context {
	public:
		Context();
		~Context();

		Context(const Context&) = delete;
		Context(Context&&) = delete;

		Context& operator=(const Context&) = delete;
		Context& operator=(Context&&) = delete;

		Error openFile(const char* szVideoFileName, Stream& stream, avcodec::Codec& codec);

		friend class Stream;

	private:
		AVFormatContext* m_pContext;
	};
}

#endif // HELPER_AVFORMAT_HELPER_H_

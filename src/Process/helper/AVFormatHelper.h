#ifndef HELPER_AVFORMAT_HELPER_H_
#define HELPER_AVFORMAT_HELPER_H_

extern "C" {
	#include <libavformat/avformat.h>
}

namespace avcodec {
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

	struct Stream {
		AVStream* pStream = nullptr;
		int iIndex = -1;
	};

	class Context {
	public:
		Context();
		~Context();

		Context(const Context&) = delete;
		Context(Context&&) = delete;

		Context& operator=(const Context&) = delete;
		Context& operator=(Context&&) = delete;

		const Stream& getVideoStream() const;

		Error openFile(const char* szVideoFileName, avcodec::Context& codecContext);

	private:
		AVFormatContext* m_pContext;
		Stream m_videoStream;
	};
}

#endif // HELPER_AVFORMAT_HELPER_H_

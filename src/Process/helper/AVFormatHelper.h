#ifndef HELPER_AVFORMAT_HELPER_H_
#define HELPER_AVFORMAT_HELPER_H_

extern "C" {
	struct AVCodec;
	struct AVFormatContext;
	struct AVPacket;
	struct AVStream;
}

namespace helper {
	namespace avformat {
		enum class Error {
			Success,
			InvalidInputFile,
			NoStreamInfo,
			NoVideoStream,
			EndOfFile,
			Unkown,
		};

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
			const AVCodec* getCodec() const;

			Error openFile(const char* szVideoFileName);
			Error readVideoFrame(AVPacket& packet);

		private:
			AVFormatContext* m_pContext;
			Stream m_videoStream;
			const AVCodec* m_pCodec;
		};
	}
}

#endif // HELPER_AVFORMAT_HELPER_H_

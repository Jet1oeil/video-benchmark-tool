#ifndef HELPER_AVCODEC_HELPER_H_
#define HELPER_AVCODEC_HELPER_H_

extern "C" {
	#include <libavcodec/avcodec.h>
}

#include <QByteArray>

namespace avformat {
	class Context;
}

namespace avcodec {
	enum class Error {
		Success,
		NoMemory,
		CopyParameters,
		OpenCodec,
		SendPacket,
		ReceiveFrame,
		EndOfFile,
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
		Error decodePacket(avformat::Context& formatContext, QByteArray& yuvBytes);

	private:
		Error decodeVideoFrame(const AVPacket* pPacket, QByteArray& yuvBytes);

	private:
		AVCodecContext* m_pContext;
		AVCodec* m_pCodec;
		AVPacket* m_pPacket;
		AVFrame* m_pFrame;
	};
}

#endif // HELPER_AVCODEC_HELPER_H_

#ifndef HELPER_AVCODEC_HELPER_H_
#define HELPER_AVCODEC_HELPER_H_

extern "C" {
	#include <libavcodec/avcodec.h>
}

#include <QByteArray>
#include <QString>

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
		SendFrame,
		ReceiveFrame,
		CodecFlushed,
		NoEncoderFound,
		Unkown,
	};

	struct EncoderParameters {
		EncoderParameters();

		int iWidth;
		int iHeight;
		AVRational timeBase;
		AVPixelFormat pixelFormat;
		AVColorRange colorRange;
		int iCRF;
		QString szPreset;
	};

	class Context {
	public:
		Context();
		~Context();

		Context(const Context&) = delete;
		Context(Context&&) = delete;

		Context& operator=(const Context&) = delete;
		Context& operator=(Context&&) = delete;

		EncoderParameters getCodecParameters() const;
		void setCodec(const AVCodec* pCodec);

		Error openDecoder(const avformat::Context& formatContext);
		Error decodeVideoFile(const char* szVideoFileName, QVector<QByteArray>& yuvFrames);
		Error decodePacket(avformat::Context& formatContext, QVector<QByteArray>& yuvFrames);

		Error openEncoder(const char* szCodecName, const EncoderParameters& parameters);
		Error encodeFrameStream(const QVector<QByteArray>& yuvFrames, const EncoderParameters& parameters, QVector<QByteArray>& packets);
		Error encodeFrame(const QByteArray& yuvFrame, QVector<QByteArray>& packets);

	private:
		Error allocateContext();
		void setPixelFormantAndColorRange(EncoderParameters& parameters) const;

		Error decodeVideoFrame(const AVPacket* pPacket, QVector<QByteArray>& yuvFrames);
		Error encodeVideoFrame(const AVFrame* pFrame, QVector<QByteArray>& packets);

	private:
		AVCodecContext* m_pContext;
		const AVCodec* m_pCodec;
		AVPacket* m_pPacket;
		AVFrame* m_pFrame;
	};
}

#endif // HELPER_AVCODEC_HELPER_H_

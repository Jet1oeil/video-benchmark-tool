#ifndef HELPER_AVCODEC_HELPER_H_
#define HELPER_AVCODEC_HELPER_H_

#include <QByteArray>
#include <QString>

#include "CodecParameters.h"

extern "C" {
	struct AVCodecContext;
	struct AVCodec;
	struct AVPacket;
	struct AVFrame;
}

namespace helper {
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
			NoCodecFound,
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

			CodecParameters getCodecParameters() const;

			const AVCodec* getCodec() const;
			void setCodec(const AVCodec* pCodec);

			Error decodeVideoFile(const char* szVideoFileName, QVector<QByteArray>& yuvFrames);
			Error decodePacketStream(QVector<QByteArray>& packets, const AVCodec* pCodec, QVector<QByteArray>& yuvFrames);

			Error encodeFrameStream(const QVector<QByteArray>& yuvFrames, const CodecParameters& parameters, CodecType codecType, int iCRF, const QString& szPreset, QVector<QByteArray>& packets);

		private:
			Error allocateContext();

			Error openDecoder(const avformat::Context& formatContext);
			Error openDecoder(const AVCodec* pCodec);
			Error decodeVideoFrame(const AVPacket* pPacket, QVector<QByteArray>& yuvFrames);
			Error decodePacket(avformat::Context& formatContext, QVector<QByteArray>& yuvFrames);

			Error openEncoder(const CodecParameters& parameters, CodecType codecType, int iCRF, const QString& szPreset);
			Error encodeVideoFrame(const AVFrame* pFrame, QVector<QByteArray>& packets);
			Error encodeFrame(const QByteArray& yuvFrame, QVector<QByteArray>& packets);

		private:
			AVCodecContext* m_pContext;
			const AVCodec* m_pCodec;
			AVPacket* m_pPacket;
			AVFrame* m_pFrame;
		};
	}
}

#endif // HELPER_AVCODEC_HELPER_H_

#ifndef HELPER_AVCODEC_HELPER_H_
#define HELPER_AVCODEC_HELPER_H_

#include <QByteArray>
#include <QString>

#include "Types/Codec.h"

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

			types::CodecParameters getCodecParameters() const;

			Error decodeVideoFile(const char* szVideoFileName, QVector<QByteArray>& yuvFrames);
			Error decodePacketStream(QVector<QByteArray>& packets, types::CodecType codecType, QVector<QByteArray>& yuvFrames);

			Error encodeFrameStream(
				const QVector<QByteArray>& yuvFrames,
				const types::CodecParameters& parameters,
				const types::EncoderParameters& encoderParameters,
				QVector<QByteArray>& packets
			);

		private:
			Error allocateContext(const AVCodec* pCodec);

			Error openDecoder(const avformat::Context& formatContext);
			Error openDecoder(types::CodecType codecType);
			Error decodeVideoFrame(const AVPacket* pPacket, QVector<QByteArray>& yuvFrames);
			Error decodePacket(avformat::Context& formatContext, QVector<QByteArray>& yuvFrames);

			Error openEncoder(const types::CodecParameters& parameters, const types::EncoderParameters& encoderParameters);
			Error encodeVideoFrame(const AVFrame* pFrame, QVector<QByteArray>& packets);
			Error encodeFrame(const QByteArray& yuvFrame, QVector<QByteArray>& packets);

		private:
			AVCodecContext* m_pContext;
			AVPacket* m_pPacket;
			AVFrame* m_pFrame;
		};
	}
}

#endif // HELPER_AVCODEC_HELPER_H_

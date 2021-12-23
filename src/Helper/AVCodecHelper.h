#ifndef HELPER_AVCODEC_HELPER_H_
#define HELPER_AVCODEC_HELPER_H_

#include "Types/Codec.h"
#include "Types/Packet.h"

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

			Error decodeVideoFile(const char* szVideoFileName, types::PacketList& yuvFrames);
			Error decodePacketStream(types::PacketList& packets, types::CodecType codecType, types::PacketList& yuvFrames);

			Error encodeFrameStream(
				const types::PacketList& yuvFrames,
				const types::CodecParameters& parameters,
				const types::EncoderParameters& encoderParameters,
				types::PacketList& packets
			);

		private:
			Error allocateContext(const AVCodec* pCodec);

			Error openDecoder(const avformat::Context& formatContext);
			Error openDecoder(types::CodecType codecType);
			Error decodeVideoFrame(const AVPacket* pPacket, types::PacketList& yuvFrames);
			Error decodePacket(avformat::Context& formatContext, types::PacketList& yuvFrames);

			Error openEncoder(const types::CodecParameters& parameters, const types::EncoderParameters& encoderParameters);
			Error encodeVideoFrame(const AVFrame* pFrame, types::PacketList& packets);
			Error encodeFrame(const types::Packet& yuvFrame, types::PacketList& packets);

		private:
			AVCodecContext* m_pContext;
			AVPacket* m_pPacket;
			AVFrame* m_pFrame;
		};
	}
}

#endif // HELPER_AVCODEC_HELPER_H_

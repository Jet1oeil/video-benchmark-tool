/* Copyright (c) 2022 Jet1oeil
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef HELPER_AVCODEC_HELPER_H_
#define HELPER_AVCODEC_HELPER_H_

#include <mutex>

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
			std::int64_t m_currentPTS;

		private:
			static bool s_bCallbackDefined;
			static std::mutex s_logMutext;
		};
	}
}

#endif // HELPER_AVCODEC_HELPER_H_

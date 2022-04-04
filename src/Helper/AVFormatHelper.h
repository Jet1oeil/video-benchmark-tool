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

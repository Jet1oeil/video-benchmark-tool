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

#include "AVFormatHelper.h"

extern "C" {
	#include <libavformat/avformat.h>
}

#include "AVCodecHelper.h"

namespace helper {
	namespace avformat {
		Context::Context()
		: m_pContext(nullptr)
		, m_pCodec(nullptr)
		{

		}

		Context::~Context()
		{
			if (m_pContext != nullptr) {
				avformat_close_input(&m_pContext);
			}
		}

		const Stream& Context::getVideoStream() const
		{
			return m_videoStream;
		}

		const AVCodec* Context::getCodec() const
		{
			return m_pCodec;
		}

		Error Context::openFile(const char* szVideoFileName)
		{
			if (avformat_open_input(&m_pContext, szVideoFileName, nullptr, nullptr) != 0) {
				return Error::InvalidInputFile;
			}

			if (avformat_find_stream_info(m_pContext, nullptr) < 0) {
				return Error::NoStreamInfo;
			}

			AVCodec* pCodec = nullptr;
			int iRes = av_find_best_stream(m_pContext, AVMEDIA_TYPE_VIDEO, -1, -1, &pCodec, 0);
			if (iRes < 0) {
				return Error::NoVideoStream;
			}

			m_pCodec = pCodec;

			m_videoStream.pStream = m_pContext->streams[iRes];
			m_videoStream.iIndex = iRes;

			return Error::Success;
		}

		Error Context::readVideoFrame(AVPacket& packet)
		{
			Error error = Error::Success;
			int iRes = 0;

			while (iRes >= 0) {
				iRes = av_read_frame(m_pContext, &packet);

				// End of file or error
				// TODO: handle error case properly
				if (iRes < 0) {
					error = Error::EndOfFile;
					break;
				}

				// Video frame reached
				if (m_videoStream.iIndex == packet.stream_index) {
					break;
				}
			}

			return error;
		}
	}
}

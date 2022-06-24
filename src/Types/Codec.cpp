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

#include "Codec.h"

namespace types {
	const char* getCodecName(CodecType codec)
	{
		int iCodec = static_cast<int>(codec);

		if (iCodec == -1) {
			return "undefined";
		}

		return CodecList[iCodec];
	}


	CodecType getCodecID(const std::string& codec)
	{
		if (codec == CodecList[0]) {
			return CodecType::X264Baseline;
		} else if (codec == CodecList[1]) {
			return CodecType::X264Main;
		} else if (codec == CodecList[2]) {
			return CodecType::X264High;
		} else if (codec == CodecList[3]) {
			return CodecType::X265Main;
		} else if (codec == CodecList[4]) {
			return CodecType::OpenH264Baseline;
		} else if (codec == CodecList[5]) {
			return CodecType::OpenH264High;
		} else if (codec == CodecList[6]) {
			return CodecType::VP8;
		}

		return CodecType::Undefined;
	}
}

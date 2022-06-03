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

#ifndef TYPE_CODEC_H_
#define TYPE_CODEC_H_

#include <array>
#include <string>
#include <vector>

#include "Vector.h"

namespace types {
	enum class CodecType {
		X264Baseline,
		X264Main,
		X264High,
		X265Main,
		Undefined,
	};

	const char* getCodecName(CodecType codec);
	CodecType getCodecID(const std::string& codec);

	// TODO: Only support yuv 420p
	enum class PixelFormat {
		YUV420P,
		Undefined,
	};

	enum class ColorRange {
		Undefined,
		MPEG,       ///< the normal 219*2^(n-8) "MPEG" YUV ranges
		JPEG,       ///< the normal     2^n-1   "JPEG" YUV ranges,
	};

	constexpr std::array<const char*, 4> CodecList = {
		"x264 - baseline",
		"x264 - main",
		"x264 - high",
		"x265 - main",
	};

	constexpr std::array<const char*, 10> PresetList = {
		"ultrafast",
		"superfast",
		"veryfast",
		"faster",
		"fast",
		"medium",
		"slow",
		"slower",
		"veryslow",
		"placebo",
	};

	struct CodecParameters {
		Vector2i videoSize;
		PixelFormat pixelFormat;
		ColorRange colorRange;
		int iPixelDepth;

		int iFPS;
	};

	struct EncoderParameters {
		CodecType codecType;
		int iCRF;
		std::string szPreset;
	};
}

using CodecList = std::vector<types::CodecType>;

#endif // TYPE_CODEC_H_

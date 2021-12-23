#ifndef TYPE_CODEC_H_
#define TYPE_CODEC_H_

#include <string>
#include <vector>

#include "Vector.h"

namespace types {
	enum class CodecType {
		H264Baseline,
		H264Main,
		H264High,
		H265Main,
		Undefined,
	};

	const char* getCodecName(CodecType codec);

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

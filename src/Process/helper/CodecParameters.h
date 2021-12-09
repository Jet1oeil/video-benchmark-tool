#ifndef HELPER_CODEC_PARAMETER_H_
#define HELPER_CODEC_PARAMETER_H_

#include "Types/Vector.h"

namespace helper {
	namespace avcodec {
		enum class CodecType {
			H265,
		};

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

	}
}

#endif // HELPER_CODEC_PARAMETER_H_

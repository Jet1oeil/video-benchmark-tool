#include "CodecParameters.h"

namespace helper {
	namespace avcodec {
		const char* getCodecName(CodecType codec)
		{
			switch (codec) {
				case CodecType::H264Baseline:
					return "H.264 basline";

				case CodecType::H264Main:
					return "H.264 main";

				case CodecType::H264High:
					return "H.264 high";

				case CodecType::H265Main:
					return "H.265 main";

				case CodecType::Undefined:
					return "undefined";
			}

			return "undefined";
		}
	}
}

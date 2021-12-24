#include "Codec.h"

namespace types {
	const char* getCodecName(CodecType codec)
	{
		switch (codec) {
			case CodecType::H264Baseline:
				return CodecList[0];

			case CodecType::H264Main:
				return CodecList[1];

			case CodecType::H264High:
				return CodecList[2];

			case CodecType::H265Main:
				return CodecList[3];

			case CodecType::Undefined:
				return "undefined";
		}

		return "undefined";
	}
}

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


	CodecType getCodecID(const std::string& codec)
	{
		if (codec == CodecList[0]) {
			return CodecType::H264Baseline;
		} else if (codec == CodecList[1]) {
			return CodecType::H264Main;
		} else if (codec == CodecList[2]) {
			return CodecType::H264High;
		} else if (codec == CodecList[3]) {
			return CodecType::H265Main;
		}

		return CodecType::Undefined;
	}
}

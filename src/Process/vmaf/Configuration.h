#ifndef VMAF_CONFIGURATION_H_
#define VMAF_CONFIGURATION_H_

#include <QString>

#include "Process/helper/CodecParameters.h"

namespace vmaf {
	struct Configuration : public helper::avcodec::EncoderParameters {
		bool operator<(const Configuration& other) const;
	};
}

#endif // VMAF_CONFIGURATION_H_

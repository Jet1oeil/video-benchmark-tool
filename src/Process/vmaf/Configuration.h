#ifndef VMAF_CONFIGURATION_H_
#define VMAF_CONFIGURATION_H_

#include <QString>

#include "Types/Codec.h"

namespace vmaf {
	struct Configuration : public types::EncoderParameters {
		bool operator<(const Configuration& other) const;
	};
}

#endif // VMAF_CONFIGURATION_H_

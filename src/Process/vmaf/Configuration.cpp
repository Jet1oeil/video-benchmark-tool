#include "Configuration.h"

#include <tuple>

namespace vmaf {
	bool Configuration::operator<(const Configuration& other) const
	{
		std::tuple<int, int, std::string> tuple{ static_cast<int>(codecType), iCRF, szPreset };
		std::tuple<int, int, std::string> otherTuple{ static_cast<int>(other.codecType), other.iCRF, other.szPreset };

		return tuple < otherTuple;
	}
}

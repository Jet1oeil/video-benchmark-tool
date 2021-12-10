#include "Configuration.h"

#include <tuple>

namespace vmaf {
	bool Configuration::operator<(const Configuration& other) const
	{
		std::tuple<int, QString> tuple{ iCRF, szPreset };
		std::tuple<int, QString> otherTuple{ other.iCRF, other.szPreset };

		return tuple < otherTuple;
	}
}

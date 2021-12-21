#include "Data.h"

#include <cassert>
#include <fstream>

#include <json.hpp>

using json = nlohmann::json;

namespace local {
	bool Configuration::operator<(const Configuration& other) const
	{
		std::tuple<std::string, std::string, int> lhs = {szCodecName, szPreset, iCRF};
		std::tuple<std::string, std::string, int> rhs = {other.szCodecName, other.szPreset, other.iCRF};

		return lhs < rhs;
	}

	ResultsMap loadResults(const std::filesystem::path& filePath)
	{
		ResultsMap map;

		// Open json file
		std::ifstream file(filePath, std::ios::in);
		assert(file.good());

		json j;
		file >> j;

		for (const auto& experiment: j["experiments"] ) {
			Configuration config = {
				experiment["key"]["codec_name"],
				experiment["key"]["preset"],
				experiment["key"]["crf"]
			};

			Results results = {
				experiment["results"]["vmaf"],
				experiment["results"]["bitstream_size"],
				experiment["results"]["encoding_time"]
			};

			auto [it, ack] = map.insert({ config, results});
			assert(ack);
		}

		return map;
	}
}

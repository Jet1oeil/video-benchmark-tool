#include "Data.h"

#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <json.hpp>

using json = nlohmann::json;

namespace {
	// from https://github.com/GamedevFramework/gf/blob/f9587d46add949191fb075f143472033e51920c9/library/core/StringUtils.cc
	std::string niceNum(float num, float precision) {
		float accpow = std::floor(std::log10(precision));

		int digits = 0;

		if (num < 0) {
			digits = static_cast<int>(std::fabs(num / std::pow(10, accpow) - 0.5f));
		} else {
			digits = static_cast<int>(std::fabs(num / std::pow(10, accpow) + 0.5f));
		}

		std::string result;

		if (digits > 0) {
			int curpow = static_cast<int>(accpow);

			for (int i = 0; i < curpow; ++i) {
			result += '0';
			}

			while (digits > 0) {
			char adigit = (digits % 10) + '0';

			if (curpow == 0 && result.length() > 0) {
				result += '.';
				result += adigit;
			} else {
				result += adigit;
			}

			digits /= 10;
			curpow += 1;
			}

			for (int i = curpow; i < 0; ++i) {
			result += '0';
			}

			if (curpow <= 0) {
			result += ".0";
			}

			if (num < 0) {
			result += '-';
			}

			std::reverse(result.begin(), result.end());
		} else {
			result = "0";
		}

		return result;
	}
}

namespace local {
	std::string Configuration::toString() const
	{
		std::ostringstream ss;
		ss << std::setw(2) << std::setfill('0') << static_cast<int>(iCRF);

		std::string name = szCodecName + "-" + szPreset + "-crf-" + ss.str();

		// Trim space
		name.erase(std::remove(name.begin(), name.end(), ' '), name.end());

		return name;
	}

	bool ExperimentResultEntry::operator<(const ExperimentResultEntry& other)
	{
		std::tuple<std::string, std::string, int> ltuple(config.szCodecName, config.szPreset, config.iCRF);
		std::tuple<std::string, std::string, int> rtuple(other.config.szCodecName, other.config.szPreset, other.config.iCRF);

		return ltuple < rtuple;
	}

	bool ExperimentResultEntry::operator==(const ExperimentResultEntry& other)
	{
		std::tuple<std::string, std::string, int> ltuple(config.szCodecName, config.szPreset, config.iCRF);
		std::tuple<std::string, std::string, int> rtuple(other.config.szCodecName, other.config.szPreset, other.config.iCRF);

		return ltuple == rtuple;
	}

	ExperimentResults loadExperimentResults(const std::filesystem::path& filePath)
	{
		ExperimentResults results;

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

			ExperimentResult result = {
				experiment["results"]["vmaf"],
				experiment["results"]["encoding_time"],
				experiment["results"]["bitstream_size"]
			};

			results.push_back({ config, result });
		}

		return results;
	}

	void fixedVMAFPareto(ExperimentResults resultsCopy, double vmafLimit)
	{
		// Remove config which exceed the encoding time limit or with insufisent quality
		resultsCopy.erase(std::remove_if(resultsCopy.begin(), resultsCopy.end(), [vmafLimit](const auto& entry) {
			return entry.result.dVMAF < vmafLimit || entry.result.dVMAF > vmafLimit + 1;
		}), resultsCopy.end());

		// Remove duplicate
		std::sort(resultsCopy.begin(), resultsCopy.end());
		std::unique(resultsCopy.begin(), resultsCopy.end());

		// Sort by bitstream size
		std::sort(resultsCopy.begin(), resultsCopy.end(), [](const auto& lhs, const auto& rhs) {
			return lhs.result.iBitstreamSize < rhs.result.iBitstreamSize;
		});

		// Write results to a file
		std::ofstream dataFile("fixed-vmaf-" + niceNum(vmafLimit, 1) + ".dat");

		for (const auto& result: resultsCopy) {
			// Config name
			dataFile << result.config.toString() << "\t" << result.result.dVMAF << "\t" << result.result.iBitstreamSize << "\t" << result.result.iEncodingTime << std::endl;
		}
	}
}

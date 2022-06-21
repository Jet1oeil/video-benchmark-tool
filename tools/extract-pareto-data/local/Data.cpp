/* Copyright (c) 2022 Jet1oeil
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Data.h"

#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
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

	std::string paddingNum(int num, int padding) {
		std::stringstream ss;
		ss << std::setw(padding) << std::setfill('0') << num;

		return ss.str();
	}
}

namespace local {
	std::string Configuration::toString() const
	{
		std::string name = szCodecName;

		if (szPreset != "none") {
			name += "-" + szPreset;
		}

		if (iCRF >= 0) {
			name += "-crf-" + paddingNum(iCRF, 2);
		}

		if (iBitrate >= 0) {
			name += "-bitrate-" + paddingNum(iBitrate, 7);
		}

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

	ExperimentResults loadExperimentResults(const std::filesystem::path& filePath, double originalSize, double originalDuration)
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
				experiment["key"]["crf"],
				experiment["key"]["bitrate"]
			};

			ExperimentResult result = {
				experiment["results"]["vmaf"],
				static_cast<double>(experiment["results"]["encoding_time"]) / originalDuration,
				static_cast<double>(experiment["results"]["bitstream_size"]) / originalSize
			};

			results.push_back({ config, result });
		}

		return results;
	}

	void fixedVMAFPareto(ExperimentResults resultsCopy, double limit, const std::string& prefix)
	{
		// Remove config which exceed the encoding time limit or with insufisent quality
		resultsCopy.erase(std::remove_if(resultsCopy.begin(), resultsCopy.end(), [limit](const auto& entry) {
			return entry.result.VMAF < limit;
		}), resultsCopy.end());

		if (resultsCopy.empty()) {
			std::cout << "No results for this contraint: " << prefix << " -> VMAF < " << std::to_string(limit) << std::endl;
			return;
		}

		assert(resultsCopy.size() > 0);

		// Remove duplicate
		std::sort(resultsCopy.begin(), resultsCopy.end());
		std::unique(resultsCopy.begin(), resultsCopy.end());

		// Sort by bitstream size
		std::sort(resultsCopy.begin(), resultsCopy.end(), [](const auto& lhs, const auto& rhs) {
			return lhs.result.bitstreamSize < rhs.result.bitstreamSize;
		});

		// Write results to a file
		std::ofstream dataFile(prefix + "fixed-vmaf-" + niceNum(limit, 1) + ".dat");

		double minEncoding = resultsCopy[0].result.encodingTime;
		for (const auto& result: resultsCopy) {
			if (result.result.encodingTime <= minEncoding) {
				minEncoding = result.result.encodingTime;
				dataFile << result.config.toString() << "\t" << result.result.VMAF << "\t" << result.result.bitstreamSize << "\t" << result.result.encodingTime << std::endl;
			}
		}
	}

	void fixedEncodingTime(ExperimentResults resultsCopy, double limit, const std::string& prefix)
	{
		// Remove config which exceed the encoding time limit or with insufisent quality
		resultsCopy.erase(std::remove_if(resultsCopy.begin(), resultsCopy.end(), [limit](const auto& entry) {
			return entry.result.encodingTime > limit;
		}), resultsCopy.end());

		if (resultsCopy.empty()) {
			std::cout << "No results for this contraint: " << prefix << " -> encoding < " << std::to_string(limit) << std::endl;
			return;
		}

		assert(resultsCopy.size() > 0);

		// Remove duplicate
		std::sort(resultsCopy.begin(), resultsCopy.end());
		std::unique(resultsCopy.begin(), resultsCopy.end());

		// Sort by bitstream size
		std::sort(resultsCopy.begin(), resultsCopy.end(), [](const auto& lhs, const auto& rhs) {
			return lhs.result.bitstreamSize < rhs.result.bitstreamSize;
		});

		// Write results to a file
		std::ofstream dataFile(prefix + "fixed-encoding-time-" + niceNum(limit, 0.1) + "x.dat");

		double maxQuality = resultsCopy[0].result.VMAF;
		for (const auto& result: resultsCopy) {
			if (result.result.VMAF >= maxQuality) {
				maxQuality = result.result.VMAF;
				dataFile << result.config.toString() << "\t" << result.result.VMAF << "\t" << result.result.bitstreamSize << "\t" << result.result.encodingTime << std::endl;
			}
		}
	}

	void fixedBitstreamsize(ExperimentResults resultsCopy, double limit, const std::string& prefix)
	{
		// Remove config which exceed the encoding time limit or with insufisent quality
		resultsCopy.erase(std::remove_if(resultsCopy.begin(), resultsCopy.end(), [limit](const auto& entry) {
			return entry.result.bitstreamSize > limit;
		}), resultsCopy.end());

		if (resultsCopy.empty()) {
			std::cout << "No results for this contraint: " << prefix << " -> bitstream size < " << std::to_string(limit) << std::endl;
			return;
		}

		// Remove duplicate
		std::sort(resultsCopy.begin(), resultsCopy.end());
		std::unique(resultsCopy.begin(), resultsCopy.end());

		// Sort by bitstream size
		std::sort(resultsCopy.begin(), resultsCopy.end(), [](const auto& lhs, const auto& rhs) {
			return lhs.result.encodingTime < rhs.result.encodingTime;
		});

		// Write results to a file
		std::ofstream dataFile(prefix + "fixed-bitstream-size-" + niceNum(limit, 0.1) + "x.dat");

		double maxQuality = resultsCopy[0].result.VMAF;
		for (const auto& result: resultsCopy) {
			if (result.result.VMAF >= maxQuality) {
				maxQuality = result.result.VMAF;
				dataFile << result.config.toString() << "\t" << result.result.VMAF << "\t" << result.result.bitstreamSize << "\t" << result.result.encodingTime << std::endl;
			}
		}
	}

	void separatedCodec(ExperimentResults resultsCopy)
	{
		std::map<std::string, std::vector<ExperimentResultEntry>> resultsByCodecName;
		for (const auto& result: resultsCopy) {
			resultsByCodecName[result.config.szCodecName].push_back(result);
		}

		std::cout << "number of different codecs: " << resultsByCodecName.size() << std::endl;

		// Iterate over codecs
		for (const auto& [codec, results]: resultsByCodecName) {
			for (int i = 70; i <= 95; i += 5) {
				local::fixedVMAFPareto(results, i, codec);
			}

			for (double i = 0.1; i <= 1.5; i += 0.1) {
				local::fixedEncodingTime(results, i, codec);
			}

			for (double i = 0.5; i <= 1.5; i += 0.1) {
				local::fixedBitstreamsize(results, i, codec);
			}
		}
	}
}

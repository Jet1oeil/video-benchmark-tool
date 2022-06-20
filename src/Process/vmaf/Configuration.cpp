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

#include "Configuration.h"

#include <filesystem>
#include <fstream>
#include <tuple>

#include <json.hpp>

#include "Helper/Log.h"

#include "Benchmark.h"

namespace fs = std::filesystem;

using json = nlohmann::json;

namespace vmaf {
	bool Configuration::operator<(const Configuration& other) const
	{
		std::tuple<int, int, std::string, int> tuple{ static_cast<int>(codecType), iCRF, szPreset, iBitrate };
		std::tuple<int, int, std::string, int> otherTuple{ static_cast<int>(other.codecType), other.iCRF, other.szPreset, other.iBitrate };

		return tuple < otherTuple;
	}

	bool updateConfigurationListFile(const std::vector<Configuration> &listConfigurations)
	{
		json jDocument;

		std::ifstream configFile(Benchmark::CurrentConfigList);

		if (!configFile.good()) {
			return false;
		}

		configFile >> jDocument;

		std::string videoSource = jDocument["video_source"];

		return writeConfigurationList(videoSource, listConfigurations);
	}

	bool writeConfigurationList(const std::string& videoFile, const std::vector<Configuration> &listConfigurations)
	{
		json jDocument;

		jDocument["video_source"] = videoFile;

		// Dump configuration list to a json file
		for (const auto& configuration: listConfigurations) {
			jDocument["configurations"].push_back(
				{
					{ "codec_type", configuration.codecType },
					{ "bitrate", configuration.iBitrate },
					{ "crf", configuration.iCRF },
					{ "preset", configuration.szPreset }
				}
			);
		}

		std::ofstream configFile(Benchmark::CurrentConfigList);

		if (!configFile.good()) {
			return false;
		}

		configFile << jDocument;

		return true;
	}

	bool readConfigurationList(std::string& szVideoFileName, std::vector<Configuration> &listConfigurations)
	{
		std::ifstream configFile(Benchmark::CurrentConfigList);

		if (!configFile.good()) {
			return false;
		}

		json jDocument;
		configFile >> jDocument;

		// Load the video source
		szVideoFileName = jDocument["video_source"];

		// Load remaining configuration from the previous run
		for (const auto& configuration: jDocument["configurations"]) {
			Configuration resumeConfig;
			resumeConfig.codecType = configuration["codec_type"];
			resumeConfig.iCRF = configuration["crf"];
			resumeConfig.szPreset = configuration["preset"];
			resumeConfig.iBitrate = configuration["bitrate"];

			listConfigurations.push_back(resumeConfig);
		}

		return true;
	}
}

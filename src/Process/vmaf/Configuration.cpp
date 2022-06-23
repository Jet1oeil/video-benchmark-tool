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

namespace {
	// Default openh264 bitrate
	const std::vector<int> listDefaultOpenH264Bitrate = {
		200000,
		400000,
		600000,
		800000,
		1200000,
		1400000,
		1600000,
		1800000,
		2000000,
		2500000,
		3000000,
		3500000,
		4000000,
		5000000,
		6000000,
		7000000,
		8000000,
	};

	std::vector<types::CodecType> generateProfiles(const std::string& codecName, const std::vector<std::string>& defaultProfiles, json document = json())
	{
		std::vector<types::CodecType> listCodec;
		helper::Log::info("\t- profiles:");

		std::vector<std::string> listProfiles = defaultProfiles;

		// Read the specified profiles if available
		if (document.contains("profile")) {
			listProfiles.clear();

			const auto& listJSONProfiles = document["profile"];
			assert(listJSONProfiles.is_array());

			// Get all profiles
			for (const auto& profileJSON: listJSONProfiles) {
				assert(profileJSON.is_string());
				std::string profile = profileJSON;
				listProfiles.push_back(profile);
			}
		}

		for (const auto& profile: listProfiles) {
			// Check if the codec is valid
			types::CodecType codecID = types::getCodecID(codecName + " - " + profile);
			if (codecID == types::CodecType::Undefined) {
				helper::Log::fatal("Invalid codec: '%s - %s'", codecName.c_str(), profile.c_str());
			}

			helper::Log::info("\t\t- %s", profile.c_str());
			listCodec.push_back(codecID);
		}

		return listCodec;
	}

	std::pair<int, int> generateCRFBounds(const std::pair<int, int>& defaultBounds, json document = json())
	{
		std::pair<int, int> bounds = defaultBounds;

		if (document.contains("crf")) {
			bounds.first = document["crf"]["min"];
			bounds.second = document["crf"]["max"];
		}
		if (bounds.first > bounds.second) {
			std::swap(bounds.first, bounds.second);
		}

		helper::Log::info("\t- crf:");
		helper::Log::info("\t\t- min: %d", bounds.first);
		helper::Log::info("\t\t- max: %d", bounds.second);

		return bounds;
	}

	std::vector<std::string> generatePresets(std::vector<std::string> defaultPresets, json document = json())
	{
		std::vector<std::string> listPresets = defaultPresets;
		if (document.contains("preset")) {
			listPresets.clear();

			const auto& listJSONPresets = document["preset"];
			assert(listJSONPresets.is_array());

			// Check all presets
			for (const auto& presetJSON: listJSONPresets) {
				assert(presetJSON.is_string());
				std::string preset = presetJSON;

				if (std::find(defaultPresets.begin(), defaultPresets.end(), preset) == defaultPresets.end()) {
					helper::Log::fatal("Invalid preset: '%s'", preset.c_str());
				}

				listPresets.push_back(preset);
			}
		}

		// Display selected preset
		helper::Log::info("\t- preset:");
		for (const auto& preset: listPresets) {
			helper::Log::info("\t\t- %s", preset.c_str());
		}

		return listPresets;
	}

	std::vector<int> generateBitrate(std::vector<int> defaultBitrates, json document = json())
	{
		std::vector<int> listBitrate = defaultBitrates;

		if (document.contains("bitrate")) {
			listBitrate.clear();

			const auto& listJSONBitrate = document["bitrate"];
			assert(listJSONBitrate.is_array());

			// Check all presets
			for (auto bitrateJSON: listJSONBitrate) {
				assert(bitrateJSON.is_number_integer());
				int bitrate = bitrateJSON;

				if (bitrate < 0) {
					helper::Log::fatal("Invalid preset: %d", bitrate);
				}

				listBitrate.push_back(bitrate);
			}
		}

		// Display selected preset
		helper::Log::info("\t- bitrate:");
		for (auto bitrate: listBitrate) {
			helper::Log::info("\t\t- %d", bitrate);
		}

		return listBitrate;
	}
}

namespace vmaf {
	bool Configuration::operator<(const Configuration& other) const
	{
		std::tuple<int, int, std::string, int> tuple{ static_cast<int>(codecType), iCRF, szPreset, iBitrate };
		std::tuple<int, int, std::string, int> otherTuple{ static_cast<int>(other.codecType), other.iCRF, other.szPreset, other.iBitrate };

		return tuple < otherTuple;
	}

	std::vector<Configuration> generateAllConfigurations()
	{
		std::vector<Configuration> listConfigurations;

		helper::Log::info("x264 parameters:");

		// Get profiles
		auto listCodec = generateProfiles("x264", {"baseline", "main", "high"});

		// Get CRF bounds
		auto crfBounds = generateCRFBounds({0, 51});

		// Get preset
		auto listPresets = generatePresets(types::PresetList);

		// Create all x264 configurations
		generateConfigurations(listConfigurations, listCodec, crfBounds, listPresets);

		helper::Log::info(" ");

		helper::Log::info("x265 parameters:");

		// Get CRF bounds
		crfBounds = generateCRFBounds({0, 51});

		// Get preset
		listPresets = generatePresets(types::PresetList);

		// Create all x265 configurations
		generateConfigurations(listConfigurations, { types::CodecType::X265Main }, crfBounds, listPresets);

		helper::Log::info(" ");

		helper::Log::info("openh264 parameters:");

		// Get profiles
		listCodec = generateProfiles("openh264", {"baseline", "high"});

		// Get bitrate
		auto listBitrate = generateBitrate(listDefaultOpenH264Bitrate);

		// Create all openh264 configurations
		generateConfigurations(listConfigurations, listCodec, listBitrate);

		return listConfigurations;
	}

	std::vector<Configuration> generateConfigurationsFromJSON(const std::filesystem::path& configFile)
	{
		std::vector<Configuration> listConfigurations;

		// Load JSON file
		std::ifstream file(configFile);

		json configJSON;

		file >> configJSON;

		if (configJSON.contains("x264")) {
			helper::Log::info("x264 parameters:");

			json x264Params = configJSON["x264"];

			// Get profiles
			auto listCodec = generateProfiles("x264", {"baseline", "main", "high"}, x264Params);

			// Get CRF bounds
			auto crfBounds = generateCRFBounds({0, 51}, x264Params);

			// Get preset
			auto listPresets = generatePresets(types::PresetList, x264Params);

			// Create all x265 configurations
			generateConfigurations(listConfigurations, listCodec, crfBounds, listPresets);

			helper::Log::info(" ");
		}

		if (configJSON.contains("x265")) {
			helper::Log::info("x265 parameters:");

			json x265Params = configJSON["x265"];

			// Get CRF bounds
			auto crfBounds = generateCRFBounds({0, 51}, x265Params);

			// Get preset
			auto listPresets = generatePresets(types::PresetList, x265Params);

			// Create all openh264 configurations
			generateConfigurations(listConfigurations, { types::CodecType::X265Main }, crfBounds, listPresets);

			helper::Log::info(" ");
		}

		if (configJSON.contains("openh264")) {
			helper::Log::info("openh264 parameters:");

			json openh264Params = configJSON["openh264"];

			// Get profiles
			auto listCodec = generateProfiles("openh264", {"baseline", "high"}, openh264Params);

			// Get bitrate
			auto listBitrate = generateBitrate(listDefaultOpenH264Bitrate, openh264Params);

			// Create all x264 configurations
			generateConfigurations(listConfigurations, listCodec, listBitrate);
		}

		return listConfigurations;
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

	// x264 / x265
	void generateConfigurations(std::vector<Configuration>& listConfigurations, const std::vector<types::CodecType>& listCodec, const std::pair<int, int>& crfBounds, const std::vector<std::string>& listPresets) {
		for (auto codec: listCodec) {
			for (auto preset: listPresets) {
				for (int i = crfBounds.first; i <= crfBounds.second; ++i) {
					vmaf::Configuration config;
					config.codecType = codec;
					config.szPreset = preset;
					config.iCRF = i;

					// Unused parameter
					config.iBitrate = -1;

					listConfigurations.push_back(config);
				}
			}
		}
	}

	// OpenH264
	void generateConfigurations(std::vector<Configuration>& listConfigurations, const std::vector<types::CodecType>& listCodec, const std::vector<int>& listBitrate) {
		for (auto codec: listCodec) {
			for (auto bitrate: listBitrate) {
				vmaf::Configuration config;
				config.codecType = codec;
				config.iBitrate = bitrate;

				// Unused parameter
				config.iCRF = -1;
				config.szPreset = "none";

				listConfigurations.push_back(config);
			}
		}
	}
}

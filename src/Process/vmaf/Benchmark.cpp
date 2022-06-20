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

#include "Benchmark.h"

#include <clocale>
#include <ctime>
#include <fstream>
#include <mutex>

#include <json.hpp>

#include "Helper/AVCodecHelper.h"
#include "Helper/AVFormatHelper.h"
#include "Helper/Log.h"

#include "Configuration.h"

namespace fs = std::filesystem;

using json = nlohmann::json;

namespace vmaf {
	const std::filesystem::path Benchmark::DumpDir = "dump";
	const std::filesystem::path Benchmark::CurrentConfigList = fs::temp_directory_path() / "vmaf-benchmark-configs.json";

	void Benchmark::reset()
	{
		m_experiment.release();
		m_results.clear();
	}

	void Benchmark::abort()
	{
		m_experiment->abort();
	}

	void Benchmark::start(
		const std::string& szVideoFileName,
		const CodecList& listCodec,
		std::pair<int, int> crfRange,
		std::vector<int> listBitrate,
		const std::vector<std::string>& listPreset,
		std::function<void()> callback
	)
	{
		helper::Log::info("selected video: %s", szVideoFileName.c_str());
		helper::Log::info("selected codec:");
		for (const auto& codecID: listCodec) {
			helper::Log::info("\t%s", types::getCodecName(codecID));
		}

		// Show openh264 parameters
		if (std::find_if(listCodec.begin(), listCodec.end(), [](const auto& val) {
				return val == types::CodecType::OpenH264Baseline || val == types::CodecType::OpenH264High;
			}) != listCodec.end()) {
			helper::Log::info("OpenH264 config:");
			helper::Log::info("bitrate:");
			for (const auto& bitrate: listBitrate) {
				helper::Log::info("\t%d", bitrate);
			}
			helper::Log::info(" "); // newline
		}

		// Show x264 / x265 parameters
		if (std::find_if(listCodec.begin(), listCodec.end(), [](const auto& codecID) {
			return (codecID == types::CodecType::X264Baseline) || (codecID == types::CodecType::X264Main) || (codecID == types::CodecType::X264Main) || (codecID == types::CodecType::X265Main);
		}) != listCodec.end()) {
			helper::Log::info("x264 / x265 config:");
			helper::Log::info("CRF: [%d - %d]", crfRange.first, crfRange.second);
			helper::Log::info("selected preset:");
			for (const auto& szPreset: listPreset) {
				helper::Log::info("\t%s", szPreset.c_str());
			}
			helper::Log::info(" "); // newline
		}

		std::vector<Configuration> listConfigurations;

		// Generate all configuration
		for (const auto& codecID: listCodec) {
			// No CRF or preset for OpenH264 just bitrate
			if (codecID == types::CodecType::OpenH264Baseline || codecID == types::CodecType::OpenH264High) {
				for (const auto& iBitrate: listBitrate) {
					listConfigurations.push_back({ codecID, -1, "none", iBitrate });
				}
			}
			// No bitrate for x264 / x265  just CRF and preset
			else {
				for (int iCRF = crfRange.first; iCRF <= crfRange.second; ++iCRF) {
					for (const auto& szPreset: listPreset) {
						listConfigurations.push_back({ codecID, iCRF, szPreset, -1 });
					}
				}
			}
		}

		// Dump configuration
		if (!writeConfigurationList(szVideoFileName, listConfigurations)) {
			helper::Log::error("Unable to open a temporary file...");
			return;
		}

		runExperiments(szVideoFileName, listConfigurations, callback);
	}

	void Benchmark::resume(std::function<void()> callback)
	{
		// Load the previous json configuration list
		std::string szVideoFileName;
		std::vector<Configuration> listConfigurations;
		bool res = readConfigurationList(szVideoFileName, listConfigurations);
		if (!res || szVideoFileName.empty() || listConfigurations.empty()) {
			helper::Log::error("Unable to load the previous experiment steps...");
			return;
		}

		// Load the previous results from dump dir
		for (const auto& dirEntry : fs::directory_iterator(DumpDir)) {
			if (dirEntry.is_regular_file() && dirEntry.path().extension() == ".json") {
				fs::path resultPath = dirEntry.path();

				json experimentJSON;
				std::ifstream resultFile(resultPath);
				if (!resultFile.good()) {
					helper::Log::error("Unable to open the previous result file: '%s'", resultPath.c_str());
					return;
				}

				resultFile >> experimentJSON;

				assert(experimentJSON["experiments"].size() == 1);
				json configJSON = experimentJSON["experiments"][0]["key"];
				json resultsJSON = experimentJSON["experiments"][0]["results"];

				Configuration prevConfig;
				prevConfig.codecType = configJSON["codec_id"];
				prevConfig.iBitrate = configJSON["bitrate"];
				prevConfig.iCRF = configJSON["crf"];
				prevConfig.szPreset = configJSON["preset"];

				Results prevResult;
				prevResult.dDecdodingTime = resultsJSON["decoding_time"];
				prevResult.dEncodingTime = resultsJSON["encoding_time"];
				prevResult.dVMAFScore = resultsJSON["vmaf"];
				prevResult.iBitstreamSize = resultsJSON["bitstream_size"];

				auto [_, inserted] = m_results.insert(std::make_pair(prevConfig, prevResult));
				if (!inserted) {
					helper::Log::error("Unable to insert previous results...");
					return;
				}
			}
		}

		helper::Log::info("Load %lu previous results...", m_results.size());

		runExperiments(szVideoFileName, listConfigurations, callback);
	}

	bool Benchmark::decodeOriginalVideoFile(const std::string& szVideoFileName, types::PacketList& yuvFrames)
	{
		helper::avcodec::Context codecContex;

		if (codecContex.decodeVideoFile(szVideoFileName.c_str(), yuvFrames) != helper::avcodec::Error::Success) {
			helper::Log::error("Error decode video...");
			return false;
		}

		m_originalCodecParameters = codecContex.getCodecParameters();

		return true;
	}

	void Benchmark::runExperiments(
		const std::string& szVideoFileName,
		const std::vector<Configuration>& listConfigurations,
		std::function<void()> callback
	)
	{
		// Decode original video
		types::PacketList yuvFrames;
		if (!decodeOriginalVideoFile(szVideoFileName, yuvFrames)) {
			helper::Log::error("Error decoding...");
		}

		// Create video dump directory -- skiping if we resume a run
		if (m_results.empty()) {
			fs::remove_all(DumpDir);
			fs::create_directory(DumpDir);
		}

		// Keep previous locale
		std::string szCurrentNumericLocale = std::setlocale(LC_NUMERIC, nullptr);
		std::string szCurrentTimeLocale = std::setlocale(LC_TIME, nullptr);

		// Set locale to 'C' to avoid bug during vmaf model loading
		// Must be done before threads launch since it's not thread-safe
		std::setlocale(LC_NUMERIC, "C");
		std::setlocale(LC_TIME, "C");

		// Alloc the thread
		std::mutex mutexExperiments;
		m_experiment = std::make_unique<Experiment>(yuvFrames, m_originalCodecParameters, listConfigurations, mutexExperiments, callback);

		m_experiment->start();

		m_experiment->wait();

		// Append new results to old results (for the resume case)
		auto experimentResults = m_experiment->getResults();
		m_results.insert(experimentResults.begin(), experimentResults.end());

		// Format filename
		std::array<char, 256> dateTimeText = { 0 };

		std::time_t currentTime = std::time(nullptr);
		std::strftime(dateTimeText.data(), dateTimeText.size(), "%Y-%m-%d-%H%M%S-results.json", std::localtime(&currentTime));

		// Print results
		vmaf::writeResult(dateTimeText.data(), m_results);

		// Restore the previous locale
		std::setlocale(LC_NUMERIC, szCurrentNumericLocale.c_str());
		std::setlocale(LC_TIME, szCurrentTimeLocale.c_str());
	}
}

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

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace vmaf {
	const std::filesystem::path Benchmark::DumpDir = "dump";

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
		std::pair<int, int> bitrateRange,
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
		if (std::find(listCodec.begin(), listCodec.end(), types::CodecType::OpenH264Baseline) != listCodec.end()) {
			helper::Log::info("OpenH264 config:");
			helper::Log::info("bitrate: [%d - %d]", bitrateRange.first, bitrateRange.second);
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

		types::PacketList yuvFrames;
		if (!decodeOriginalVideoFile(szVideoFileName, yuvFrames)) {
			helper::Log::error("Error decoding...");
		}

		// Create video dump directory
		fs::remove_all(DumpDir);
		fs::create_directory(DumpDir);

		runExperiments(yuvFrames, listCodec, crfRange, bitrateRange, listPreset, callback);
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
		const types::PacketList& yuvFrames,
		const CodecList& listCodec,
		std::pair<int, int> crfRange,
		std::pair<int, int> bitrateRange,
		const std::vector<std::string>& listPreset,
		std::function<void()> callback
	)
	{
		std::vector<Configuration> listConfigurations;

		// Generate all configuration
		for (const auto& codecID: listCodec) {
			// No CRF or preset for OpenH264 just bitrate
			if (codecID == types::CodecType::OpenH264Baseline) {
				// Bitrate by step of 100 Kbit
				for (int iBitrate = bitrateRange.first; iBitrate <= bitrateRange.second; iBitrate += 100) {
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
		m_results = m_experiment->getResults();

		// Print results
		json jsonDocument;
		for (const auto& [key, value]: m_results) {
			helper::Log::debug("[Codec=%d, CRF=%d, preset=%s, bitrate=%d]: VMAF=%f", static_cast<int>(key.codecType), key.iCRF, key.szPreset.c_str(), key.iBitrate, value.dVMAFScore);
			json jKey = {
				"key", {
					{ "codec_id", static_cast<int>(key.codecType) },
					{ "codec_name", types::getCodecName(key.codecType) },
					{ "crf", key.iCRF },
					{ "preset", key.szPreset.c_str() },
					{ "bitrate", key.iBitrate }
				}
			};

			json jResult = {
				"results", {
					{ "vmaf", value.dVMAFScore },
					{ "bitstream_size", value.iBitstreamSize },
					{ "encoding_time", value.dEncodingTime },
					{ "decoding_time", value.dDecdodingTime },
				}
			};
			jsonDocument["experiments"].push_back(
				{ jKey, jResult }
			);
		}

		std::ofstream jsonFile;

		std::array<char, 256> dateTimeText = { 0 };

		std::time_t currentTime = std::time(nullptr);
		std::strftime(dateTimeText.data(), dateTimeText.size(), "%Y-%m-%d-%H%M%S-results.json", std::localtime(&currentTime));
		jsonFile.open(dateTimeText.data());

		if (!jsonFile.good()) {
			helper::Log::error("Unable to store results :\n%s", jsonDocument.dump(4).c_str());
			return;
		}

		jsonFile << jsonDocument;

		// Restore the previous locale
		std::setlocale(LC_NUMERIC, szCurrentNumericLocale.c_str());
		std::setlocale(LC_TIME, szCurrentTimeLocale.c_str());
	}
}

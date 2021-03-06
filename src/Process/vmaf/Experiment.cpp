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

#include "Experiment.h"

#include <cassert>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "Helper/AVCodecHelper.h"
#include "Helper/AVFormatHelper.h"
#include "Helper/VMAFWrapper.h"

#include "Helper/Log.h"

#include "Types/Clock.h"

#include "Benchmark.h"
#include "Configuration.h"
#include "Results.h"

namespace fs = std::filesystem;

namespace {
	// TODO: Add support for other format
	VmafPixelFormat convertAVPixelFormat(types::PixelFormat pixelFormat)
	{
		switch (pixelFormat) {
		case types::PixelFormat::YUV420P:
			return VMAF_PIX_FMT_YUV420P;

		case types::PixelFormat::Undefined:
			return VMAF_PIX_FMT_UNKNOWN;
		}

		return VMAF_PIX_FMT_UNKNOWN;
	}
}

namespace vmaf {
	Experiment::Experiment(
		const types::FrameList& yuvFrames,
		const types::CodecParameters& codecParameters,
		const std::vector<Configuration>& listConfigurations,
		std::mutex& mutexExperiments,
		std::function<void()> callback
	)
	: m_yuvFrames(yuvFrames)
	, m_codecParameters(codecParameters)
	, m_listConfiguration(listConfigurations)
	, m_abort(false)
	, m_mutexExperiments(mutexExperiments)
	, m_progressCallback(callback)
	{

	}

	Experiment::~Experiment()
	{
		if (m_thread.joinable()) {
			m_thread.detach();
		}
	}

	Experiment::Experiment(Experiment&& other)
	: m_yuvFrames(other.m_yuvFrames)
	, m_codecParameters(other.m_codecParameters)
	, m_listConfiguration(other.m_listConfiguration)
	, m_abort((other.m_abort) ? true : false)
	, m_mutexExperiments(other.m_mutexExperiments)
	, m_progressCallback(other.m_progressCallback)
	{

	}

	const std::map<Configuration, Results>& Experiment::getResults() const
	{
		return m_results;
	}

	void Experiment::start()
	{
		m_thread = std::move(std::thread(&Experiment::doStart, this));
	}

	void Experiment::wait()
	{
		m_thread.join();
	}

	void Experiment::abort()
	{
		helper::Log::debug("Abort...");
		m_abort = true;
	}

	void Experiment::doStart()
	{
		Configuration currentConfiguration;

		while (stoleTask(currentConfiguration)) {
			helper::Log::debug("codec: %d, CRF: %d, preset: %s, bitrate: %d",
				static_cast<int>(currentConfiguration.codecType),
				currentConfiguration.iCRF,
				currentConfiguration.szPreset.c_str(),
				currentConfiguration.iBitrate
			);

			Results results;

			// Create the file base name
			// Codec name
			std::string codecName = types::getCodecName(currentConfiguration.codecType);
			codecName.erase(std::remove_if(codecName.begin(), codecName.end(), [](auto c) {
				return std::isspace(c);
			}), codecName.end());

			// Base name
			std::string basename =
				Benchmark::DumpDir.string() +
				+ "/transcoded-video-codec-" + codecName
				+ "-preset-" + currentConfiguration.szPreset
				+ "-crf-" + helper::paddingNum(currentConfiguration.iCRF, 2)
				+ "-bitrate-" + helper::paddingNum(currentConfiguration.iBitrate, 7);

			// Encode the video
			types::PacketList packets;
			types::Clock clock;
			{
				helper::avcodec::Context encoder;

				if (encoder.encodeFrameStream(m_yuvFrames, m_codecParameters, currentConfiguration, packets) != helper::avcodec::Error::Success) {
					logAndProgress("Encode error...");
					continue;
				}
				results.iEncodingTime = clock.getDuration().count();

				std::string extension;
				if (currentConfiguration.codecType == types::CodecType::VP8 || currentConfiguration.codecType == types::CodecType::VP9) {
					extension = ".webm";
				} else {
					extension = ".mp4";
				}

				std::string filename = basename + extension;

				helper::avformat::Context muxer;
				muxer.writeOutputFile(packets, encoder.getCodeContext(), filename.c_str());
			}

			// Decode the transcoded video
			types::FrameList transcodedYUVFrames;
			{
				helper::avcodec::Context decoder;

				clock.restart();
				if (decoder.decodePacketStream(packets, currentConfiguration.codecType, transcodedYUVFrames) != helper::avcodec::Error::Success) {
					logAndProgress("Decode transcoded video error...");
					continue;
				}
				results.iDecodingTime = clock.getDuration().count();
			}

			if (m_yuvFrames.size() != transcodedYUVFrames.size()) {
				logAndProgress("Frame count mismatch...");
				continue;
			}

			// Call vmaf
			auto pixelFormat = m_codecParameters.pixelFormat;
			helper::VMAFWrapper vmaf(convertAVPixelFormat(pixelFormat), m_codecParameters.iPixelDepth, m_codecParameters.videoSize.width, m_codecParameters.videoSize.height);

			if (!vmaf.open()) {
				logAndProgress("VMAF error...");
				continue;
			}

			if (!vmaf.computeMetrics(m_yuvFrames, transcodedYUVFrames)) {
				logAndProgress("Compute VMAF error...");
				continue;
			}

			helper::Log::info("End of experiment {Codec: %s, CRF: %d, preset: %s, bitrate: %d}",
				types::getCodecName(currentConfiguration.codecType),
				currentConfiguration.iCRF,
				currentConfiguration.szPreset.c_str(),
				currentConfiguration.iBitrate
			);

			results.dVMAFScore = vmaf.getVMAFScore();

			m_results.insert({ currentConfiguration, results });

			// Generate json file path
			std::string filename = basename + ".json";

			// Write results to dumps dir
			writeResult(filename, currentConfiguration, results);

			// Remove the current configuration form the resume configuration list file
			updateConfigurationListFile(m_listConfiguration);

			// Update progress state
			if (m_progressCallback) {
				m_progressCallback();
			}
		}

		// When experiments are finished, clear the the resume configuration file
		fs::remove(Benchmark::CurrentConfigList);
	}

	bool Experiment::stoleTask(Configuration& configuration)
	{
		if (m_abort) {
			helper::Log::debug("Aborted");
			return false;
		}

		std::lock_guard<std::mutex> locker(m_mutexExperiments);

		// No last experiment
		if (m_listConfiguration.empty()) {
			return false;
		}

		// We take the last to avoid data shift
		configuration = m_listConfiguration.back();
		m_listConfiguration.pop_back();

		return true;
	}

	void Experiment::logAndProgress(const char* message)
	{
		helper::Log::error(message);
		if (m_progressCallback) {
			m_progressCallback();
		}
	}
}

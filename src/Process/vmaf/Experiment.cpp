#include "Experiment.h"

#include <cassert>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "Helper/AVCodecHelper.h"
#include "Helper/VMAFWrapper.h"

#include "Helper/Log.h"

#include "Types/Clock.h"

#include "Benchmark.h"
#include "Configuration.h"
#include "Results.h"

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
		const types::PacketList& yuvFrames,
		const types::CodecParameters& codecParameters,
		std::vector<Configuration>& listConfigurations,
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
			helper::Log::debug("codec: %d, CRF: %d, preset: %s",
				static_cast<int>(currentConfiguration.codecType),
				currentConfiguration.iCRF,
				currentConfiguration.szPreset.c_str()
			);

			Results results;

			// Encode the video
			helper::avcodec::Context encoder;
			types::PacketList packets;

			types::Clock clock;
			if (encoder.encodeFrameStream(m_yuvFrames, m_codecParameters, currentConfiguration, packets) != helper::avcodec::Error::Success) {
				logAndProgress("Encode error...");
				continue;
			}
			results.dEncodingTime = clock.getDuration().count();

			results.iBitstreamSize = 0;
			for (const auto& packet: packets) {
				results.iBitstreamSize += packet.size();
			}

			// Decode the transcoded video
			types::PacketList transcodedYUVFrames;
			helper::avcodec::Context decoder;

			clock.restart();
			if (decoder.decodePacketStream(packets, currentConfiguration.codecType, transcodedYUVFrames) != helper::avcodec::Error::Success) {
				logAndProgress("Decode transcoded video error...");
				continue;
			}
			results.dDecdodingTime = clock.getDuration().count();

			if (m_yuvFrames.size() != transcodedYUVFrames.size()) {
				logAndProgress("Frame count mismatch...");
				continue;
			}

			// Filename
			std::string codecName = types::getCodecName(currentConfiguration.codecType);
			codecName.erase(std::remove_if(codecName.begin(), codecName.end(), [](auto c) {
				return std::isspace(c);
			}), codecName.end());

			std::ostringstream filename;
			filename << std::setw(3) << std::setfill('0')
				<< "transcoded-video-codec-" << codecName
				<< "-preset-" << currentConfiguration.szPreset
				<< "-crf-" << currentConfiguration.iCRF << ".yuv";

			// Dump transcoded video
			std::ofstream dumpFile(Benchmark::DumpDir / filename.str(), std::ios::binary);
			assert(dumpFile.good());

			for (const auto& yuvFrame: transcodedYUVFrames) {
				dumpFile.write(reinterpret_cast<const char*>(yuvFrame.data()), yuvFrame.size());
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

			helper::Log::info("End of experiment {Codec: %s, CRF: %d, preset: %s}",
				types::getCodecName(currentConfiguration.codecType),
				currentConfiguration.iCRF,
				currentConfiguration.szPreset.c_str()
			);

			results.dVMAFScore = vmaf.getVMAFScore();

			m_results.insert({ currentConfiguration, results });

			// Update progress state
			if (m_progressCallback) {
				m_progressCallback();
			}
		}
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

#include "Experiment.h"

#include <sstream>

#include "Process/helper/AVCodecHelper.h"
#include "Process/helper/CodecParameters.h"
#include "Process/helper/VMAFWrapper.h"

#include "Types/Clock.h"

#include "Configuration.h"
#include "Results.h"

namespace {
	// TODO: Add support for other format
	VmafPixelFormat convertAVPixelFormat(helper::avcodec::PixelFormat pixelFormat)
	{
		switch (pixelFormat) {
		case helper::avcodec::PixelFormat::YUV420P:
			return VMAF_PIX_FMT_YUV420P;

		case helper::avcodec::PixelFormat::Undefined:
			return VMAF_PIX_FMT_UNKNOWN;
		}

		return VMAF_PIX_FMT_UNKNOWN;
	}

	inline std::string getThreadID()
	{
		std::ostringstream oss;
		oss << std::this_thread::get_id();
		return oss.str();
	}
}

namespace vmaf {
	Experiment::Experiment(
		const QVector<QByteArray>& yuvFrames,
		const helper::avcodec::CodecParameters& codecParameters,
		std::vector<Configuration>& listConfigurations,
		std::mutex& mutexExperiments
	)
	: m_yuvFrames(yuvFrames)
	, m_codecParameters(codecParameters)
	, m_listConfiguration(listConfigurations)
	, m_mutexExperiments(mutexExperiments)
	{

	}

	Experiment::Experiment(Experiment&& other)
	: m_yuvFrames(other.m_yuvFrames)
	, m_codecParameters(other.m_codecParameters)
	, m_listConfiguration(other.m_listConfiguration)
	, m_mutexExperiments(other.m_mutexExperiments)
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

	void Experiment::doStart()
	{
		Configuration currentConfiguration;

		while (stoleTask(currentConfiguration)) {
			qDebug("TID: %s, codec: %d, CRF: %d, preset: %s",
				getThreadID().c_str(),
				static_cast<int>(currentConfiguration.codecType),
				currentConfiguration.iCRF,
				qPrintable(currentConfiguration.szPreset)
			);

			Results results;

			// Encode the video
			helper::avcodec::Context encoder;
			QVector<QByteArray> packets;

			types::Clock clock;
			if (encoder.encodeFrameStream(m_yuvFrames, m_codecParameters, currentConfiguration, packets) != helper::avcodec::Error::Success) {
				qDebug("Encode error...");
				continue;
			}
			results.dEncodingTime = clock.getDuration().count();

			results.iBitstreamSize = 0;
			for (const auto& packet: packets) {
				results.iBitstreamSize += packet.count();
			}

			// Decode the transcoded video
			QVector<QByteArray> transcodedYUVFrames;
			helper::avcodec::Context decoder;

			clock.restart();
			if (decoder.decodePacketStream(packets, currentConfiguration.codecType, transcodedYUVFrames) != helper::avcodec::Error::Success) {
				qDebug("Decode transcoded video error...");
				continue;
			}
			results.dDecdodingTime = clock.getDuration().count();

			if (m_yuvFrames.size() != transcodedYUVFrames.size()) {
				qDebug("Frame count mismatch...");
				continue;
			}

			// Call vmaf
			auto pixelFormat = m_codecParameters.pixelFormat;
			helper::VMAFWrapper vmaf(convertAVPixelFormat(pixelFormat), m_codecParameters.iPixelDepth, m_codecParameters.videoSize.width, m_codecParameters.videoSize.height);

			if (!vmaf.open()) {
				qDebug("VMAF error...");
				continue;
			}

			if (!vmaf.computeMetrics(m_yuvFrames, transcodedYUVFrames)) {
				qDebug("Compute VMAF error...");
				continue;
			}

			results.dVMAFScore = vmaf.getVMAFScore();

			m_results.insert({ currentConfiguration, results });
		}
	}

	bool Experiment::stoleTask(Configuration& configuration)
	{
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
}

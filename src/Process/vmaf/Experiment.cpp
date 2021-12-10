#include "Experiment.h"

#include "Process/helper/AVCodecHelper.h"
#include "Process/helper/CodecParameters.h"
#include "Process/helper/VMAFWrapper.h"

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
}

namespace vmaf {
	ExperimentThread::ExperimentThread(
		const QVector<QByteArray>& yuvFrames,
		const helper::avcodec::CodecParameters& codecParameters,
		QVector<Configuration>& listConfigurations,
		QMutex& mutexExperiments
	)
	: m_yuvFrames(yuvFrames)
	, m_codecParameters(codecParameters)
	, m_listConfiguration(listConfigurations)
	, m_mutexExperiments(mutexExperiments)
	{

	}

	ExperimentThread::ExperimentThread(ExperimentThread&& other)
	: m_yuvFrames(other.m_yuvFrames)
	, m_codecParameters(other.m_codecParameters)
	, m_listConfiguration(other.m_listConfiguration)
	, m_mutexExperiments(other.m_mutexExperiments)
	{

	}

	const std::map<Configuration, Results>& ExperimentThread::getResults() const
	{
		return m_results;
	}

	void ExperimentThread::run()
	{
		Configuration currentConfiguration;

		while (stoleTask(currentConfiguration)) {
			qDebug("TID: %p, codec: %d, CRF: %d, preset: %s",
				QThread::currentThreadId(),
				static_cast<int>(currentConfiguration.codecType),
				currentConfiguration.iCRF,
				qPrintable(currentConfiguration.szPreset)
			);

			// Encode the video
			helper::avcodec::Context encoder;
			QVector<QByteArray> packets;

			if (encoder.encodeFrameStream(m_yuvFrames, m_codecParameters, currentConfiguration, packets) != helper::avcodec::Error::Success) {
				qDebug("Encode error...");
				continue;
			}

			// Decode the transcoded video
			QVector<QByteArray> transcodedYUVFrames;
			helper::avcodec::Context decoder;
			if (decoder.decodePacketStream(packets, currentConfiguration.codecType, transcodedYUVFrames) != helper::avcodec::Error::Success) {
				qDebug("Decode transcoded video error...");
				continue;
			}

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

			Results results;
			results.dVMAFScore = vmaf.getVMAFScore();

			m_results.insert({ currentConfiguration, results });
		}
	}

	bool ExperimentThread::stoleTask(Configuration& configuration)
	{
		QMutexLocker locker(&m_mutexExperiments);

		// No last experiment
		if (m_listConfiguration.isEmpty()) {
			return false;
		}

		configuration = m_listConfiguration.takeLast();

		return true;
	}
}

#include "Experiment.h"

#include <QFile>

#include "Process/helper/AVCodecHelper.h"

namespace vmaf {
	ExperimentThread::ExperimentThread(
		const QVector<QByteArray>& yuvFrames,
		const avcodec::EncoderParameters& encoderParameters,
		QVector<Experiment>& listExperiments,
		QMutex& mutexExperiments
	)
	: m_yuvFrames(yuvFrames)
	, m_encoderParameters(encoderParameters)
	, m_listExperiments(listExperiments)
	, m_mutexExperiments(mutexExperiments)
	{

	}

	ExperimentThread::ExperimentThread(ExperimentThread&& other)
	: m_yuvFrames(other.m_yuvFrames)
	, m_encoderParameters(other.m_encoderParameters)
	, m_listExperiments(other.m_listExperiments)
	, m_mutexExperiments(other.m_mutexExperiments)
	{

	}

	void ExperimentThread::run()
	{
		Experiment currentExperiment;

		while (stoleTask(currentExperiment)) {
			qDebug("TID: %p, CRF: %d, preset: %s", QThread::currentThreadId(), currentExperiment.iCRF, qPrintable(currentExperiment.szPreset));

			m_encoderParameters.iCRF = currentExperiment.iCRF;
			m_encoderParameters.szPreset = currentExperiment.szPreset;

			// Encode the video
			QVector<QByteArray> packets;
			if (!encodeVideo(packets)) {
				qDebug("Encode error...");
				continue;
			}
		}
	}

	bool ExperimentThread::stoleTask(Experiment& experiment)
	{
		QMutexLocker locker(&m_mutexExperiments);

		// No last experiment
		if (m_listExperiments.isEmpty()) {
			return false;
		}

		experiment = m_listExperiments.takeLast();

		return true;
	}

	bool ExperimentThread::encodeVideo(QVector<QByteArray>& packets)
	{
		avcodec::Context encoderContext;
		if (encoderContext.openEncoder("libx265", m_encoderParameters) != avcodec::Error::Success) {
			return false;
		}

		for (const auto& yuvFrame: m_yuvFrames) {
			if (encoderContext.encodeFrame(yuvFrame, packets) != avcodec::Error::Success) {
				return false;
			}
		}

		// Flush decoder
		if (encoderContext.encodeFrame(QByteArray(), packets) != avcodec::Error::EndOfFile) {
			return false;
		}

		return true;
	}
}

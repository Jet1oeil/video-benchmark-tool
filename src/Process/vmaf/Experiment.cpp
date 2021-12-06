#include "Experiment.h"

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
			avcodec::Context encoder;
			QVector<QByteArray> packets;

			if (encoder.encodeFrameStream(m_yuvFrames, m_encoderParameters, packets) != avcodec::Error::Success) {
				qDebug("Encode error...");
				continue;
			}

			// Decode the transcoded video
			QVector<QByteArray> transcodedYUVFrames;
			avcodec::Context decoder;
			if (decoder.decodePacketStream(packets, encoder.getCodec(), transcodedYUVFrames) != avcodec::Error::Success) {
				qDebug("Decode transcoded video error...");
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
}

#ifndef VMAF_EXPERIMENT_H_
#define VMAF_EXPERIMENT_H_

#include <QMutex>
#include <QString>
#include <QThread>
#include <QVector>

#include "Process/helper/AVCodecHelper.h"

namespace vmaf {
	struct Experiment {
		int iCRF;
		QString szPreset;
	};

	class ExperimentThread: public QThread {
	Q_OBJECT

	public:
		ExperimentThread(
			const QVector<QByteArray>& yuvFrames,
			const avcodec::EncoderParameters& encoderParameters,
			QVector<Experiment>& listExperiments,
			QMutex& mutexExperiments
		);
		~ExperimentThread() = default;

		ExperimentThread(const ExperimentThread&) = delete;
		ExperimentThread(ExperimentThread&& other);

		ExperimentThread& operator=(const ExperimentThread&) = delete;
		ExperimentThread& operator=(ExperimentThread&& other) = delete;

	private:
		virtual void run() override;

		bool stoleTask(Experiment& experiment);

	private:
		const QVector<QByteArray>& m_yuvFrames;
		avcodec::EncoderParameters m_encoderParameters;
		QVector<Experiment>& m_listExperiments;
		QMutex& m_mutexExperiments;
	};
}

#endif // VMAF_EXPERIMENT_H_

#ifndef VMAF_EXPERIMENT_H_
#define VMAF_EXPERIMENT_H_

#include <QMutex>
#include <QString>
#include <QThread>
#include <QVector>

#include "Process/helper/AVCodecHelper.h"

namespace vmaf {
	struct Configuration;

	class ExperimentThread: public QThread {
	Q_OBJECT

	public:
		ExperimentThread(
			const QVector<QByteArray>& yuvFrames,
			const helper::avcodec::EncoderParameters& encoderParameters,
			QVector<Configuration>& listConfiguration,
			QMutex& mutexExperiments
		);
		~ExperimentThread() = default;

		ExperimentThread(const ExperimentThread&) = delete;
		ExperimentThread(ExperimentThread&& other);

		ExperimentThread& operator=(const ExperimentThread&) = delete;
		ExperimentThread& operator=(ExperimentThread&& other) = delete;

	private:
		virtual void run() override;

		bool stoleTask(Configuration& experiment);

	private:
		const QVector<QByteArray>& m_yuvFrames;
		helper::avcodec::EncoderParameters m_encoderParameters;
		QVector<Configuration>& m_listConfiguration;
		QMutex& m_mutexExperiments;
	};
}

#endif // VMAF_EXPERIMENT_H_

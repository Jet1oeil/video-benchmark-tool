#ifndef VMAF_EXPERIMENT_H_
#define VMAF_EXPERIMENT_H_

#include <QMutex>
#include <QString>
#include <QThread>
#include <QVector>

namespace vmaf {
	struct Experiment {
		int iCRF;
		QString szPreset;
	};

	class ExperimentThread: public QThread {
	Q_OBJECT

	public:
		ExperimentThread(QVector<Experiment>& listExperiments, QMutex& mutexExperiments);
		~ExperimentThread() = default;

		ExperimentThread(const ExperimentThread&) = delete;
		ExperimentThread(ExperimentThread&& other);

		ExperimentThread& operator=(const ExperimentThread&) = delete;
		ExperimentThread& operator=(ExperimentThread&& other) = delete;

	private:
		virtual void run() override;

		bool stoleTask(Experiment& experiment);

	private:
		QVector<Experiment>& m_listExperiments;
		QMutex& m_mutexExperiments;
	};
}

#endif // VMAF_EXPERIMENT_H_

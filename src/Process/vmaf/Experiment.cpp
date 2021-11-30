#include "Experiment.h"

namespace vmaf {
	ExperimentThread::ExperimentThread(QVector<Experiment>& listExperiments, QMutex& mutexExperiments)
	: m_listExperiments(listExperiments)
	, m_mutexExperiments(mutexExperiments)
	{

	}

	ExperimentThread::ExperimentThread(ExperimentThread&& other)
	: m_listExperiments(other.m_listExperiments)
	, m_mutexExperiments(other.m_mutexExperiments)
	{

	}

	void ExperimentThread::run()
	{
		Experiment currentExperiment;

		while (stoleTask(currentExperiment)) {
			qDebug("TID: %p, CRF: %d, preset: %s", QThread::currentThreadId(), currentExperiment.iCRF, qPrintable(currentExperiment.szPreset));
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

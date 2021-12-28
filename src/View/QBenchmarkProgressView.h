#ifndef Q_BENCHMARK_PROGRESS_VIEW_H_
#define Q_BENCHMARK_PROGRESS_VIEW_H_

#include <QProgressBar>
#include <QMutex>

namespace view {

	class QBenchmarkProgressView: public QWidget {
	Q_OBJECT

	public:
		QBenchmarkProgressView(QWidget* pParent);

		void setTotalExperiment(int iTotalExperiment);

	public slots:
		void updateProgress();

	private:
		QProgressBar* m_pProgressBar;
		QMutex m_mutex;
	};

}

#endif // Q_BENCHMARK_PROGRESS_VIEW_H_

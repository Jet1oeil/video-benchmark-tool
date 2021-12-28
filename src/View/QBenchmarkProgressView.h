#ifndef Q_BENCHMARK_PROGRESS_VIEW_H_
#define Q_BENCHMARK_PROGRESS_VIEW_H_

#include <QLabel>
#include <QMutex>
#include <QPushButton>
#include <QProgressBar>

namespace view {

	class QBenchmarkProgressView: public QWidget {
	Q_OBJECT

	public:
		QBenchmarkProgressView(QWidget* pParent);

		void setTotalExperiment(int iTotalExperiment);

		void showCancelAnimation();

	signals:
		void cancelBenchmarkTriggered();

	public slots:
		void updateProgress();

	private:
		QLabel* m_pLabel;
		QProgressBar* m_pProgressBar;
		QPushButton* m_pCancelButton;
		QMutex m_mutex;
	};

}

#endif // Q_BENCHMARK_PROGRESS_VIEW_H_

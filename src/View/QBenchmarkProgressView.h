#ifndef Q_BENCHMARK_PROGRESS_VIEW_H_
#define Q_BENCHMARK_PROGRESS_VIEW_H_

#include <QProgressBar>

namespace view {

	class QBenchmarkProgressView: public QWidget {
	Q_OBJECT

	public:
		QBenchmarkProgressView(QWidget* pParent);

	private:
		QProgressBar* m_pProgressBar;
	};

}

#endif // Q_BENCHMARK_PROGRESS_VIEW_H_

#ifndef Q_BENCHMARK_CONTROLLER_H_
#define Q_BENCHMARK_CONTROLLER_H_

#include <QObject>

#include "Process/vmaf/Benchmark.h"

namespace view {
	class QMainView;
}

namespace ctrl {
	class QBenchmarkController: public QObject {
	Q_OBJECT

	public:
		QBenchmarkController(view::QMainView& mainView);

	private slots:
		void onBrowseFile();
		void onStartBenchmark();

	private:
		view::QMainView& m_mainView;
	};
}

#endif // Q_BENCHMARK_CONTROLLER_H_

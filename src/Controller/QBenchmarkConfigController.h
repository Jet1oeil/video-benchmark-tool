#ifndef Q_BENCHMARK_CONFIG_CONTROLLER_H_
#define Q_BENCHMARK_CONFIG_CONTROLLER_H_

#include <QObject>

#include "Process/vmaf/Benchmark.h"

namespace view {
	class QMainView;
}

namespace ctrl {
	class QBenchmarkConfigController: public QObject {
	Q_OBJECT

	public:
		QBenchmarkConfigController(view::QMainView& mainView);

	private slots:
		void onBrowseFile();
		void onStartBenchmark();

	private:
		view::QMainView& m_mainView;
	};
}

#endif // Q_BENCHMARK_CONFIG_CONTROLLER_H_

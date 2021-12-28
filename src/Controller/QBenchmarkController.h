#ifndef Q_BENCHMARK_CONTROLLER_H_
#define Q_BENCHMARK_CONTROLLER_H_

#include <thread>

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
		~QBenchmarkController();

		QBenchmarkController(const QBenchmarkController&) = delete;
		QBenchmarkController(QBenchmarkController&&) = delete;

		QBenchmarkController& operator=(const QBenchmarkController&) = delete;
		QBenchmarkController& operator=(QBenchmarkController&&) = delete;

	private slots:
		void onBrowseFile();
		void onStartBenchmark();
		void onCancelBenchmark();

	private:
		view::QMainView& m_mainView;
		std::thread m_thread;

		vmaf::Benchmark m_vmafBenchmark;
	};
}

#endif // Q_BENCHMARK_CONTROLLER_H_

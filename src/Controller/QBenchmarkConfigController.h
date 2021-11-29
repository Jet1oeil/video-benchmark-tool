#ifndef Q_BENCHMARK_CONFIG_CONTROLLER_H_
#define Q_BENCHMARK_CONFIG_CONTROLLER_H_

#include <QObject>

namespace view {
	class QMainView;
	class QBenchmarkConfigView;
}

namespace ctrl {
	class QBenchmarkConfigController: public QObject {
	Q_OBJECT

	public:
		QBenchmarkConfigController(view::QMainView& mainView);

	private slots:
		void onBrowseFile();

	private:
		view::QBenchmarkConfigView& m_benchmarkConfigView;
	};
}

#endif // Q_BENCHMARK_CONFIG_CONTROLLER_H_

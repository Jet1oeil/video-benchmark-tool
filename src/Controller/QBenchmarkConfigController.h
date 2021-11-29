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
		QBenchmarkConfigController(const view::QMainView& mainView);

	private slots:
		void onBrowseFile();

	private:
		const view::QBenchmarkConfigView& m_benchmarkConfigView;
	};
}

#endif // Q_BENCHMARK_CONFIG_CONTROLLER_H_

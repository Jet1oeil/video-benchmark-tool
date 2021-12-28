#ifndef Q_MAIN_VIEW_H_
#define Q_MAIN_VIEW_H_

#include <QStackedWidget>
#include <QWidget>

namespace view {
	class QBenchmarkConfigView;
	class QBenchmarkProgressView;

	class QMainView: public QWidget {
	Q_OBJECT

	public:
	QMainView();
	~QMainView() = default;

	QMainView(const QMainView&) = delete;
	QMainView(QMainView&&) = delete;

	QMainView& operator=(const QMainView&) = delete;
	QMainView& operator=(QMainView&&) = delete;

	QBenchmarkConfigView& getBenchmarkConfigView();
	QBenchmarkProgressView& getBenchmarkProgressView();

	void showConfig();
	void showProgress();

	private:
		QStackedWidget* m_pStackedWidget;

		QBenchmarkConfigView* m_pBenchmarkConfigView;
		QBenchmarkProgressView* m_pBenchmarkProgressView;
	};
}

#endif // Q_MAIN_VIEW_H_

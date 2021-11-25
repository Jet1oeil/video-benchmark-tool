#ifndef Q_MAIN_VIEW_H_
#define Q_MAIN_VIEW_H_

#include <QWidget>

namespace view {
	class QBenchmarkConfigView;

	class QMainView: public QWidget {
	Q_OBJECT

	public:
	QMainView();
	~QMainView() = default;

	QMainView(const QMainView&) = delete;
	QMainView(QMainView&&) = delete;

	QMainView& operator=(const QMainView&) = delete;
	QMainView& operator=(QMainView&&) = delete;

	private:
		QBenchmarkConfigView* m_pBenchmarkConfigView;
	};
}

#endif // Q_MAIN_VIEW_H_

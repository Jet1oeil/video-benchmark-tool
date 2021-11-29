#include "QBenchmarkConfigController.h"

#include "View/QBenchmarkConfigView.h"
#include "View/QMainView.h"

namespace ctrl {
	QBenchmarkConfigController::QBenchmarkConfigController(const view::QMainView& mainView)
	: m_benchmarkConfigView(mainView.getBenchmarkConfigView())
	{
		connect(&m_benchmarkConfigView, &view::QBenchmarkConfigView::browseFileTriggered, this, &QBenchmarkConfigController::onBrowseFile);
	}

	void QBenchmarkConfigController::onBrowseFile()
	{
		qDebug("browse file...");
	}
}

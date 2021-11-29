#include "QBenchmarkConfigController.h"

#include <QFileDialog>

#include "View/QBenchmarkConfigView.h"
#include "View/QMainView.h"

namespace ctrl {
	QBenchmarkConfigController::QBenchmarkConfigController(view::QMainView& mainView)
	: m_benchmarkConfigView(mainView.getBenchmarkConfigView())
	{
		connect(&m_benchmarkConfigView, &view::QBenchmarkConfigView::browseFileTriggered, this, &QBenchmarkConfigController::onBrowseFile);
	}

	void QBenchmarkConfigController::onBrowseFile()
	{
		QString szFileName = QFileDialog::getOpenFileName(&m_benchmarkConfigView, tr("Open Original Video"), QString(), tr("Video Files (*.avi *.mp4 *.mkv)"));

		if (!szFileName.isEmpty()) {
			m_benchmarkConfigView.setSelectedFile(szFileName);
		}
	}
}

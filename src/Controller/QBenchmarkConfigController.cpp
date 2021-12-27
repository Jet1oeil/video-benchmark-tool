#include "QBenchmarkConfigController.h"

#include <thread>

#include <QFileDialog>
#include <QMessageBox>

#include "View/QBenchmarkConfigView.h"
#include "View/QMainView.h"

namespace ctrl {
	QBenchmarkConfigController::QBenchmarkConfigController(view::QMainView& mainView)
	: m_benchmarkConfigView(mainView.getBenchmarkConfigView())
	{
		connect(&m_benchmarkConfigView, &view::QBenchmarkConfigView::browseFileTriggered, this, &QBenchmarkConfigController::onBrowseFile);
		connect(&m_benchmarkConfigView, &view::QBenchmarkConfigView::startBenchmarkTriggered, this, &QBenchmarkConfigController::onStartBenchmark);
	}

	void QBenchmarkConfigController::onBrowseFile()
	{
		QString szFileName = QFileDialog::getOpenFileName(&m_benchmarkConfigView, tr("Open Original Video"), QString(), tr("Video Files (*.avi *.mp4 *.mkv)"));

		if (!szFileName.isEmpty()) {
			m_benchmarkConfigView.setSelectedFile(szFileName);
		}
	}

	void QBenchmarkConfigController::onStartBenchmark()
	{
		QString szVideoFileName = m_benchmarkConfigView.getSelectedFile();

		if (szVideoFileName.isEmpty()) {
			QMessageBox::critical(&m_benchmarkConfigView, tr("Invalid Video File"), tr("No video file is specified").arg(szVideoFileName));
			return;
		}

		if (!QFile::exists(szVideoFileName)) {
			QMessageBox::critical(&m_benchmarkConfigView, tr("Invalid Video File"), tr("The file '%1' is invalid").arg(szVideoFileName));
			return;
		}

		auto listCodec = m_benchmarkConfigView.getSelectedCodec();

		if (listCodec.empty()) {
			QMessageBox::critical(&m_benchmarkConfigView, tr("Invalid Codec"), tr("At least one codec must be selected"));
			return;
		}

		int iMinCRF = m_benchmarkConfigView.getMinCRF();
		int iMaxCRF = m_benchmarkConfigView.getMaxCRF();

		if (iMinCRF < 0 || iMaxCRF > 51) {
			QMessageBox::critical(&m_benchmarkConfigView, tr("Invalid CRF"), tr("The CRF range is invald. It must be included in [0 - 51]"));
			return;
		}

		// Swap min max CRF if needed
		if (iMaxCRF < iMinCRF) {
			int iTmp = iMaxCRF;
			iMaxCRF = iMinCRF;
			iMinCRF = iTmp;
		}

		std::vector<std::string> listPreset = m_benchmarkConfigView.getPresetList();

		if (listPreset.empty()) {
			QMessageBox::critical(&m_benchmarkConfigView, tr("Invalid Preset"), tr("At least one preset must be selected"));
			return;
		}

		std::thread([szVideoFileName, listCodec, iMinCRF, iMaxCRF, listPreset]{
			vmaf::Benchmark vmafBenchmark;

			vmafBenchmark.start(szVideoFileName.toStdString(), listCodec, iMinCRF, iMaxCRF, listPreset);
		}).detach();
	}
}

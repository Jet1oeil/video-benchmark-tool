#include "QBenchmarkController.h"

#include <thread>

#include <QFileDialog>
#include <QMessageBox>

#include "View/QBenchmarkConfigView.h"
#include "View/QBenchmarkProgressView.h"
#include "View/QMainView.h"

namespace ctrl {
	QBenchmarkController::QBenchmarkController(view::QMainView& mainView)
	: m_mainView(mainView)
	{
		// Connect config signals
		auto& configView = mainView.getBenchmarkConfigView();
		connect(&configView, &view::QBenchmarkConfigView::browseFileTriggered, this, &QBenchmarkController::onBrowseFile);
		connect(&configView, &view::QBenchmarkConfigView::startBenchmarkTriggered, this, &QBenchmarkController::onStartBenchmark);
	}

	void QBenchmarkController::onBrowseFile()
	{
		auto& configView = m_mainView.getBenchmarkConfigView();
		QString szFileName = QFileDialog::getOpenFileName(&configView, tr("Open Original Video"), QString(), tr("Video Files (*.avi *.mp4 *.mkv)"));

		if (!szFileName.isEmpty()) {
			configView.setSelectedFile(szFileName);
		}
	}

	void QBenchmarkController::onStartBenchmark()
	{
		auto& configView = m_mainView.getBenchmarkConfigView();
		QString szVideoFileName = configView.getSelectedFile();

		if (szVideoFileName.isEmpty()) {
			QMessageBox::critical(&configView, tr("Invalid Video File"), tr("No video file is specified").arg(szVideoFileName));
			return;
		}

		if (!QFile::exists(szVideoFileName)) {
			QMessageBox::critical(&configView, tr("Invalid Video File"), tr("The file '%1' is invalid").arg(szVideoFileName));
			return;
		}

		auto listCodec = configView.getSelectedCodec();

		if (listCodec.empty()) {
			QMessageBox::critical(&configView, tr("Invalid Codec"), tr("At least one codec must be selected"));
			return;
		}

		int iMinCRF = configView.getMinCRF();
		int iMaxCRF = configView.getMaxCRF();

		if (iMinCRF < 0 || iMaxCRF > 51) {
			QMessageBox::critical(&configView, tr("Invalid CRF"), tr("The CRF range is invald. It must be included in [0 - 51]"));
			return;
		}

		// Swap min max CRF if needed
		if (iMaxCRF < iMinCRF) {
			int iTmp = iMaxCRF;
			iMaxCRF = iMinCRF;
			iMinCRF = iTmp;
		}

		std::vector<std::string> listPreset = configView.getPresetList();

		if (listPreset.empty()) {
			QMessageBox::critical(&configView, tr("Invalid Preset"), tr("At least one preset must be selected"));
			return;
		}

		// Show progress bar
		m_mainView.showProgress();

		auto callback = [this](){
			auto& progressView = m_mainView.getBenchmarkProgressView();
			progressView.updateProgress();
		};

		int iTotalExperiments = listCodec.size() * (iMaxCRF - iMinCRF + 1) * listPreset.size();
		m_mainView.getBenchmarkProgressView().setTotalExperiment(iTotalExperiments);

		std::thread([=]{
			vmaf::Benchmark vmafBenchmark;

			vmafBenchmark.start(szVideoFileName.toStdString(), listCodec, iMinCRF, iMaxCRF, listPreset, callback);
		}).detach();
	}
}

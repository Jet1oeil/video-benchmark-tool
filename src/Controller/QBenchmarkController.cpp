/* Copyright (c) 2022 Jet1oeil
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

		// Connect progress signals
		auto& progressView = mainView.getBenchmarkProgressView();
		connect(&progressView, &view::QBenchmarkProgressView::cancelBenchmarkTriggered, this, &QBenchmarkController::onCancelBenchmark);
	}

	QBenchmarkController::~QBenchmarkController()
	{
		if (m_thread.joinable()) {
			m_thread.detach();
		}
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

		// TODO: need to check previous thread ?
		m_thread = std::thread([=]{
			m_vmafBenchmark.reset();

			m_vmafBenchmark.start(szVideoFileName.toStdString(), listCodec, std::make_pair(iMinCRF, iMaxCRF), std::make_pair(200, 2000), listPreset, callback);
		});
	}

	void QBenchmarkController::onCancelBenchmark() {
		auto& progressView = m_mainView.getBenchmarkProgressView();
		progressView.showCancelAnimation();

		m_vmafBenchmark.abort();
	}
}

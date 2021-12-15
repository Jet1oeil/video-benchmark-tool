#include "Benchmark.h"

#include <clocale>
#include <fstream>

#include <QDateTime>
#include <QLocale>

#include <json.hpp>

#include "Process/helper/AVCodecHelper.h"
#include "Process/helper/AVFormatHelper.h"

#include "Configuration.h"

using json = nlohmann::json;

namespace vmaf {
	void Benchmark::start(const QString& szVideoFileName, const QCodecList& listCodec, int iMinCRF, int iMaxCRF, const QStringList& listPreset)
	{
		BenchmarkThread *pMainThread = new BenchmarkThread(szVideoFileName, listCodec, iMinCRF, iMaxCRF, listPreset);
		pMainThread->setParent(this);

		connect(pMainThread, &BenchmarkThread::benchmarkFinished, this, &Benchmark::handleBenchmarkFinished);
		connect(pMainThread, &BenchmarkThread::finished, pMainThread, &QObject::deleteLater);
		pMainThread->start();
	}

	void Benchmark::handleBenchmarkFinished()
	{
		qDebug("Benchmark finished");
	}


	BenchmarkThread::BenchmarkThread(const QString& szVideoFileName, const QCodecList& listCodec, int iMinCRF, int iMaxCRF, const QStringList& listPreset)
	: m_szVideoFileName(szVideoFileName)
	, m_listCodec(listCodec)
	, m_iMinCRF(iMinCRF)
	, m_iMaxCRF(iMaxCRF)
	, m_listPreset(listPreset)
	{

	}

	void BenchmarkThread::run()
	{
		qDebug("selected video: %s", qPrintable(m_szVideoFileName));
		qDebug("selected codec:");
		for (const auto& codecID: m_listCodec) {
			qDebug("\t%d", static_cast<int>(codecID));
		}
		qDebug("CRF: [%d - %d]", m_iMinCRF, m_iMaxCRF);
		qDebug("selected preset:");
		for (const auto& szPreset: m_listPreset) {
			qDebug("\t%s", qPrintable(szPreset));
		}

		QVector<QByteArray> yuvFrames;
		if (!decodeOriginalVideoFile(m_szVideoFileName, yuvFrames)) {
			qDebug("Error decoding...");
		}

		runExperiments(yuvFrames);

		emit benchmarkFinished();
	}

	bool BenchmarkThread::decodeOriginalVideoFile(const QString& szVideoFileName, QVector<QByteArray>& yuvFrames)
	{
		helper::avcodec::Context codecContex;

		if (codecContex.decodeVideoFile(qPrintable(szVideoFileName), yuvFrames) != helper::avcodec::Error::Success) {
			qDebug("Error decode video...");
			return false;
		}

		m_originalCodecParameters = codecContex.getCodecParameters();

		return true;
	}

	void BenchmarkThread::runExperiments(const QVector<QByteArray>& yuvFrames)
	{
		QVector<Configuration> listConfigurations;

		// Generate all configuration
		for (const auto& codecID: m_listCodec) {
			for (int iCRF = m_iMinCRF; iCRF <= m_iMaxCRF; ++iCRF) {
				for (const auto& szPreset: m_listPreset) {
					listConfigurations.append({ codecID, iCRF, szPreset });
				}
			}
		}

		// Keep previous locale
		std::string szCurrentLocale = std::setlocale(LC_NUMERIC, nullptr);

		// Set locale to 'C' to avoid bug during vmaf model loading
		// Must be done before threads launch since it's not thread-safe
		std::setlocale(LC_NUMERIC, "C");

		// Alloc the thread pool
		QMutex mutexExperiments;
		for (int i = 0; i < QThread::idealThreadCount(); ++i) {
			m_poolThreads.emplace_back(ExperimentThread(yuvFrames, m_originalCodecParameters, listConfigurations, mutexExperiments));
		}

		// Start all threads
		for (auto& thread: m_poolThreads) {
			thread.start();
		}

		// Join all thread
		for (auto& thread: m_poolThreads) {
			thread.wait();
			auto threadResults = thread.getResults();
			m_results.insert(threadResults.begin(), threadResults.end());
		}

		// Restore the previous locale
		std::setlocale(LC_NUMERIC, szCurrentLocale.c_str());

		// Print results
		json jsonDocument;
		for (const auto& [key, value]: m_results) {
			qDebug("[Codec=%d, CRF=%d, preset=%s]: VMAF=%f", static_cast<int>(key.codecType), key.iCRF, qPrintable(key.szPreset), value.dVMAFScore);
			json jKey = {
				"key", {
					{ "codec_id", static_cast<int>(key.codecType) },
					{ "codec_name", helper::avcodec::getCodecName(key.codecType) },
					{ "crf", key.iCRF },
					{ "preset", qPrintable(key.szPreset) }
				}
			};

			json jResult = {
				"results", {
					{ "vmaf", value.dVMAFScore },
					{ "bitstream_size", value.iBitstreamSize },
					{ "encoding_time", value.dEncodingTime },
					{ "decoding_time", value.dDecdodingTime },
				}
			};
			jsonDocument["experiments"].push_back(
				{ jKey, jResult }
			);
		}

		std::ofstream jsonFile;

		QLocale enLocale = QLocale(QLocale::English, QLocale::UnitedStates);
		QString dateTimeText = enLocale.toString(QDateTime::currentDateTime(), "yyyy-MM-dd-HHmmss");
		jsonFile.open(dateTimeText.toStdString() + "-results.json");

		if (!jsonFile.good()) {
			qCritical("Unable to store results :\n%s", jsonDocument.dump(4).c_str());
			return;
		}

		jsonFile << jsonDocument;
	}
}

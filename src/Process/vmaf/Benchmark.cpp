#include "Benchmark.h"

#include <clocale>

#include "Process/helper/AVCodecHelper.h"
#include "Process/helper/AVFormatHelper.h"

#include "Configuration.h"

namespace vmaf {
	void Benchmark::start(const QString& szVideoFileName, int iMinCRF, int iMaxCRF, const QStringList& listPreset)
	{
		BenchmarkThread *pMainThread = new BenchmarkThread(szVideoFileName, iMinCRF, iMaxCRF, listPreset);
		pMainThread->setParent(this);

		connect(pMainThread, &BenchmarkThread::benchmarkFinished, this, &Benchmark::handleBenchmarkFinished);
		connect(pMainThread, &BenchmarkThread::finished, pMainThread, &QObject::deleteLater);
		pMainThread->start();
	}

	void Benchmark::handleBenchmarkFinished()
	{
		qDebug("Benchmark finished");
	}


	BenchmarkThread::BenchmarkThread(const QString& szVideoFileName, int iMinCRF, int iMaxCRF, const QStringList& listPreset)
	: m_szVideoFileName(szVideoFileName)
	, m_iMinCRF(iMinCRF)
	, m_iMaxCRF(iMaxCRF)
	, m_listPreset(listPreset)
	{

	}

	void BenchmarkThread::run()
	{
		qDebug("selected video: %s", qPrintable(m_szVideoFileName));
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
		avcodec::Context codecContex;

		if (codecContex.decodeVideoFile(qPrintable(szVideoFileName), yuvFrames) != avcodec::Error::Success) {
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
		for (int iCRF = m_iMinCRF; iCRF <= m_iMaxCRF; ++iCRF) {
			for (const auto& szPreset: m_listPreset) {
				listConfigurations.append({ iCRF, szPreset });
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
		}

		// Restore the previous locale
		std::setlocale(LC_NUMERIC, szCurrentLocale.c_str());
	}
}

#include "BenchmarkVMAF.h"

#include "Decoder.h"

namespace proc {
	WorkerVMAF::WorkerVMAF(const QString& szVideoFileName, int iMinCRF, int iMaxCRF, const QStringList& listPreset)
	: m_szVideoFileName(szVideoFileName)
	, m_iMinCRF(iMinCRF)
	, m_iMaxCRF(iMaxCRF)
	, m_listPreset(listPreset)
	{

	}

	void WorkerVMAF::run()
	{
		qDebug("selected video: %s", qPrintable(m_szVideoFileName));
		qDebug("CRF: [%d - %d]", m_iMinCRF, m_iMaxCRF);
		qDebug("selected preset:");
		for (const auto& szPreset: m_listPreset) {
			qDebug("\t%s", qPrintable(szPreset));
		}

		QByteArray yuvBytes;
		if (!Decoder::decodeVideoFile(m_szVideoFileName, yuvBytes)) {
			qDebug("Error decoding...");
		}

		QThread::sleep(5);

		emit benchmarkFinished();
	}

	void BenchmarkVMAF::start(const QString& szVideoFileName, int iMinCRF, int iMaxCRF, const QStringList& listPreset)
	{
		WorkerVMAF *pWorker = new WorkerVMAF(szVideoFileName, iMinCRF, iMaxCRF, listPreset);
		pWorker->setParent(this);

		connect(pWorker, &WorkerVMAF::benchmarkFinished, this, &BenchmarkVMAF::handleBenchmarkFinished);
		connect(pWorker, &WorkerVMAF::finished, pWorker, &QObject::deleteLater);
		pWorker->start();
	}

	void BenchmarkVMAF::handleBenchmarkFinished()
	{
		qDebug("Benchmark finished");
	}
}

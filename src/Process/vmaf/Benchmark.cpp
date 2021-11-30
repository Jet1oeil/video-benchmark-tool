#include "Benchmark.h"

#include "Process/helper/AVCodecHelper.h"
#include "Process/helper/AVFormatHelper.h"

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

		QByteArray yuvBytes;
		if (!decodeVideoFile(m_szVideoFileName, yuvBytes)) {
			qDebug("Error decoding...");
		}

		QThread::sleep(5);

		emit benchmarkFinished();
	}

	bool BenchmarkThread::decodeVideoFile(const QString& szVideoFileName, QByteArray& yuvBytes)
	{
		avformat::Context formatContext;
		avcodec::Context codecContex;

		if (formatContext.openFile(qPrintable(szVideoFileName), codecContex) != avformat::Error::Success) {
			qDebug("Error format...");
			return false;
		}

		if (codecContex.open(formatContext) != avcodec::Error::Success) {
			qDebug("Error codec...");
			return false;
		}

		avcodec::Error errorCodec = avcodec::Error::Success;
		do {
			errorCodec = codecContex.decodePacket(formatContext, yuvBytes);
		} while (errorCodec == avcodec::Error::Success);

		if (errorCodec != avcodec::Error::Success && errorCodec != avcodec::Error::EndOfFile) {
			qDebug("Error decode...");
			return false;
		}

		return true;
	}
}

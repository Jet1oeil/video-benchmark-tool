#ifndef VMAF_BENCHMARK_H_
#define VMAF_BENCHMARK_H_

#include <QObject>
#include <QStringList>
#include <QString>
#include <QThread>

namespace vmaf {
	class Benchmark: public QObject {
	Q_OBJECT

	public:
		void start(const QString& szVideoFileName, int iMinCRF, int iMaxCRF, const QStringList& listPreset);

	private slots:
		void handleBenchmarkFinished();
	};

	class BenchmarkThread : public QThread {
	Q_OBJECT

	public:
		BenchmarkThread(const QString& szVideoFileName, int iMinCRF, int iMaxCRF, const QStringList& listPreset);

	signals:
		void benchmarkFinished();

	private:
		virtual void run() override;

		bool decodeVideoFile(const QString& szVideoFileName, QByteArray& yuvBytes);

	private:
		QString m_szVideoFileName;
		int m_iMinCRF;
		int m_iMaxCRF;
		QStringList m_listPreset;
	};
}

#endif // BENCHMARK_VMAF_H_

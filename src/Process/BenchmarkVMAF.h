#ifndef BENCHMARK_VMAF_H_
#define BENCHMARK_VMAF_H_

#include <QObject>
#include <QStringList>
#include <QString>
#include <QThread>

namespace proc {
	class WorkerVMAF : public QThread {
	Q_OBJECT

	public:
		WorkerVMAF(const QString& szVideoFileName, int iMinCRF, int iMaxCRF, const QStringList& listPreset);

	signals:
		void benchmarkFinished();

	private:
		virtual void run() override;

	private:
		QString m_szVideoFileName;
		int m_iMinCRF;
		int m_iMaxCRF;
		QStringList m_listPreset;
	};

	class BenchmarkVMAF: public QObject {
	Q_OBJECT

	public:
		void start(const QString& szVideoFileName, int iMinCRF, int iMaxCRF, const QStringList& listPreset);

	private slots:
		void handleBenchmarkFinished();
	};
}

#endif // BENCHMARK_VMAF_H_

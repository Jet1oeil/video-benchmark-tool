#ifndef VMAF_BENCHMARK_H_
#define VMAF_BENCHMARK_H_

#include <map>
#include <vector>

#include <QStringList>
#include <QString>

#include "Types/Codec.h"

#include "Configuration.h"
#include "Experiment.h"
#include "Results.h"

namespace vmaf {
	class Benchmark {
	public:
		void start(const QString& szVideoFileName, const CodecList& listCodec, int iMinCRF, int iMaxCRF, const QStringList& listPreset);

	private:
		bool decodeOriginalVideoFile(const QString& szVideoFileName, QVector<QByteArray>& yuvFrames);
		void runExperiments(const QVector<QByteArray>& yuvFrames, const CodecList& listCodec, int iMinCRF, int iMaxCRF, const QStringList& listPreset);

	private:
		types::CodecParameters m_originalCodecParameters;

		std::vector<Experiment> m_poolThreads;
		std::map<Configuration, Results> m_results;
	};
}

#endif // BENCHMARK_VMAF_H_

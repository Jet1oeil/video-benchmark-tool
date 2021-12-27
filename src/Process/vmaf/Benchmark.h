#ifndef VMAF_BENCHMARK_H_
#define VMAF_BENCHMARK_H_

#include <map>
#include <string>
#include <vector>

#include "Types/Codec.h"
#include "Types/Packet.h"

#include "Configuration.h"
#include "Experiment.h"
#include "Results.h"

namespace vmaf {
	class Benchmark {
	public:
		Benchmark();

		void setThreadCount(unsigned iCount);

		void start(const std::string& szVideoFileName, const CodecList& listCodec, int iMinCRF, int iMaxCRF, const std::vector<std::string>& listPreset);

	private:
		bool decodeOriginalVideoFile(const std::string& szVideoFileName, types::PacketList& yuvFrames);
		void runExperiments(const types::PacketList& yuvFrames, const CodecList& listCodec, int iMinCRF, int iMaxCRF, const std::vector<std::string>& listPreset);

	private:
		types::CodecParameters m_originalCodecParameters;
		unsigned m_iThreadCount;

		std::vector<Experiment> m_poolThreads;
		std::map<Configuration, Results> m_results;
	};
}

#endif // BENCHMARK_VMAF_H_

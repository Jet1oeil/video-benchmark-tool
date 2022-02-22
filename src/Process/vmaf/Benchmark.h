#ifndef VMAF_BENCHMARK_H_
#define VMAF_BENCHMARK_H_

#include <filesystem>
#include <functional>
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
		void reset();

		void start(
			const std::string& szVideoFileName,
			const CodecList& listCodec,
			int iMinCRF,
			int iMaxCRF,
			const std::vector<std::string>& listPreset,
			std::function<void()> callback = std::function<void()>()
		);

		void abort();

	public:
		static const std::filesystem::path DumpDir;

	private:
		bool decodeOriginalVideoFile(const std::string& szVideoFileName, types::PacketList& yuvFrames);
		void runExperiments(
			const types::PacketList& yuvFrames,
			const CodecList& listCodec,
			int iMinCRF,
			int iMaxCRF,
			const std::vector<std::string>& listPreset,
			std::function<void()> callback
		);

	private:
		types::CodecParameters m_originalCodecParameters;

		std::unique_ptr<Experiment> m_experiment;
		std::map<Configuration, Results> m_results;
	};
}

#endif // BENCHMARK_VMAF_H_

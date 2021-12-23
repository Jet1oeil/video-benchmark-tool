#ifndef VMAF_EXPERIMENT_H_
#define VMAF_EXPERIMENT_H_

#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "Types/Codec.h"
#include "Types/Packet.h"

namespace vmaf {
	struct Configuration;
	struct Results;

	class Experiment {
	public:
		Experiment(
			const types::PacketList& yuvFrames,
			const types::CodecParameters& codecParameters,
			std::vector<Configuration>& listConfiguration,
			std::mutex& mutexExperiments
		);
		~Experiment() = default;

		Experiment(const Experiment&) = delete;
		Experiment(Experiment&& other);

		Experiment& operator=(const Experiment&) = delete;
		Experiment& operator=(Experiment&&) = delete;

		const std::map<Configuration, Results>& getResults() const;

		void start();
		void wait();

	private:
		void doStart();

		bool stoleTask(Configuration& experiment);

	private:
		const types::PacketList& m_yuvFrames;
		types::CodecParameters m_codecParameters;
		std::vector<Configuration>& m_listConfiguration;

		std::thread m_thread;
		std::mutex& m_mutexExperiments;

		std::map<Configuration, Results> m_results;
	};
}

#endif // VMAF_EXPERIMENT_H_

#ifndef VMAF_EXPERIMENT_H_
#define VMAF_EXPERIMENT_H_

#include <atomic>
#include <functional>
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
			std::mutex& mutexExperiments,
			std::function<void()> callback
		);
		~Experiment();

		Experiment(const Experiment&) = delete;
		Experiment(Experiment&& other);

		Experiment& operator=(const Experiment&) = delete;
		Experiment& operator=(Experiment&&) = delete;

		const std::map<Configuration, Results>& getResults() const;

		void start();
		void wait();
		void abort();

	private:
		void doStart();

		bool stoleTask(Configuration& experiment);

		void logAndProgress(const char* message);

	private:
		const types::PacketList& m_yuvFrames;
		types::CodecParameters m_codecParameters;
		std::vector<Configuration>& m_listConfiguration;

		std::atomic_bool m_abort;
		std::thread m_thread;
		std::mutex& m_mutexExperiments;

		std::map<Configuration, Results> m_results;

		std::function<void()> m_progressCallback;
	};
}

#endif // VMAF_EXPERIMENT_H_

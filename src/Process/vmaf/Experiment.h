/* Copyright (c) 2022 Jet1oeil
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
			const types::FrameList& yuvFrames,
			const types::CodecParameters& codecParameters,
			const std::vector<Configuration>& listConfiguration,
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
		const types::FrameList& m_yuvFrames;
		types::CodecParameters m_codecParameters;
		std::vector<Configuration> m_listConfiguration;

		std::atomic_bool m_abort;
		std::thread m_thread;
		std::mutex& m_mutexExperiments;

		std::map<Configuration, Results> m_results;

		std::function<void()> m_progressCallback;
	};
}

#endif // VMAF_EXPERIMENT_H_

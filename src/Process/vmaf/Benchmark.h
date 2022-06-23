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

#ifndef VMAF_BENCHMARK_H_
#define VMAF_BENCHMARK_H_

#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <utility>
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
			const std::vector<Configuration>& listConfigurations,
			std::function<void()> callback = std::function<void()>()
		);

		void resume(std::function<void()> callback = std::function<void()>());

		void abort();

	public:
		static const std::filesystem::path DumpDir;
		static const std::filesystem::path CurrentConfigList;

	private:
		bool decodeOriginalVideoFile(const std::string& szVideoFileName, types::PacketList& yuvFrames);
		void runExperiments(
			const std::string& szVideoFileName,
			const std::vector<Configuration>& listConfigurations,
			std::function<void()> callback
		);

	private:
		types::CodecParameters m_originalCodecParameters;

		std::unique_ptr<Experiment> m_experiment;
		std::map<Configuration, Results> m_results;
	};
}

#endif // BENCHMARK_VMAF_H_

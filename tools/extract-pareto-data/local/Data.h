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

#ifndef LOCAL_DATA_H_
#define LOCAL_DATA_H_

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace local {
	struct Configuration {
		std::string szCodecName;
		std::string szPreset;
		int iCRF;
		int iBitrate;

		std::string toString() const;
	};

	struct ExperimentResult {
		double VMAF;
		double encodingTime;
		double bitstreamSize;
	};

	struct ExperimentResultEntry {
		Configuration config;
		ExperimentResult result;

		bool operator<(const ExperimentResultEntry& other);
		bool operator==(const ExperimentResultEntry& other);
	};

	using ExperimentResults = std::vector<ExperimentResultEntry>;

	ExperimentResults loadExperimentResults(const std::filesystem::path& filePath, double originalSize, double originalDuration);
	void fixedVMAFPareto(ExperimentResults resultsCopy, double limit, std::string prefix = "");
	void fixedEncodingTime(ExperimentResults resultsCopy, double limit, std::string prefix = "");
	void fixedBitstreamsize(ExperimentResults resultsCopy, double limit, std::string prefix = "");
	void separatedCodec(ExperimentResults resultsCopy);
}

#endif // LOCAL_DATA_H_

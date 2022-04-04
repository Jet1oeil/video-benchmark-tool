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

#include "Data.h"

#include <cassert>
#include <fstream>

#include <json.hpp>

using json = nlohmann::json;

namespace local {
	bool Configuration::operator<(const Configuration& other) const
	{
		std::tuple<std::string, std::string, int> lhs = {szCodecName, szPreset, iCRF};
		std::tuple<std::string, std::string, int> rhs = {other.szCodecName, other.szPreset, other.iCRF};

		return lhs < rhs;
	}

	ExperimentResults loadExperimentResults(const std::filesystem::path& filePath)
	{
		ExperimentResults results;

		// Open json file
		std::ifstream file(filePath, std::ios::in);
		assert(file.good());

		json j;
		file >> j;

		for (const auto& experiment: j["experiments"] ) {
			Configuration config = {
				experiment["key"]["codec_name"],
				experiment["key"]["preset"],
				experiment["key"]["crf"]
			};

			ExperimentResult result = {
				experiment["results"]["vmaf"],
				experiment["results"]["encoding_time"],
				experiment["results"]["bitstream_size"]
			};

			results.push_back({ config, result });
		}

		return results;
	}
}

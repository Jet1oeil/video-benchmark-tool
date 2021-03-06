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

#include <cassert>

#include <iostream>
#include <fstream>

#include <json.hpp>

using json = nlohmann::json;

struct VMAFResult {
	std::string preset;
	double vmaf;
};

struct BitstreamSizeResult {
	std::string preset;
	int bitstreamSize;
};

struct EncodingTimeResult {
	std::string preset;
	int encodingTime;
};

struct DecodingTimeResult {
	std::string preset;
	int decodingTime;
};

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Missing parameters" << std::endl;
		std::cerr << "Usage:" << std::endl;
		std::cerr << "\t" << argv[0] << " RESULT_FILE" << std::endl;
		return 1;
	}

	// Open json file
	std::ifstream jsonFile(std::string(argv[1]), std::ios::in);
	assert(jsonFile.good());

	json j;
	jsonFile >> j;

	// Plot VMAF graph
	for (int codecID = 0; codecID < 4; ++codecID) {
		std::map<int, std::vector<VMAFResult>> vmafResults;
		std::map<int, std::vector<BitstreamSizeResult>> bitstreamSizeResults;
		std::map<int, std::vector<EncodingTimeResult>> encodingTimeResults;
		std::map<int, std::vector<DecodingTimeResult>> decodingTimeResults;
		std::string codecName;
		for (const auto& experiment: j["experiments"] ) {
			if (experiment["key"]["codec_id"] == codecID) {
				codecName = experiment["key"]["codec_name"];
				vmafResults[experiment["key"]["crf"]].push_back({ experiment["key"]["preset"], experiment["results"]["vmaf"] });
				bitstreamSizeResults[experiment["key"]["crf"]].push_back({ experiment["key"]["preset"], experiment["results"]["bitstream_size"] });
				encodingTimeResults[experiment["key"]["crf"]].push_back({ experiment["key"]["preset"], experiment["results"]["encoding_time"] });
				decodingTimeResults[experiment["key"]["crf"]].push_back({ experiment["key"]["preset"], experiment["results"]["decoding_time"] });
			}
		}

		std::string shortCodecName;
		switch (codecID) {
		case 0:
			shortCodecName = "h264-baseline";
			break;

		case 1:
			shortCodecName = "h264-main";
			break;

		case 2:
			shortCodecName = "h264-high";
			break;

		case 3:
			shortCodecName = "h265-main";
			break;

		default:
			shortCodecName = "unknown";
		}

		{
			std::ofstream vmafFile("vmaf-" + shortCodecName + ".dat", std::ios::out);
			std::ofstream vmafPlotFile("vmaf-" + shortCodecName + "-plot.gpi", std::ios::out);

			assert(vmafFile.good());
			assert(vmafPlotFile.good());

			bool generatePlotFile = true;
			int index = 2;
			for (auto& [key, results]: vmafResults) {
				vmafFile << key;

				std::sort(results.begin(), results.end(), [](const auto& lhs, const auto& rhs) {
					return lhs.preset < rhs.preset;
				});

				if (generatePlotFile) {
					vmafPlotFile << "set terminal pdf" << std::endl;
					vmafPlotFile << "set output \"vmaf-" + shortCodecName + ".pdf\"" << std::endl;
					vmafPlotFile << "set title \"VMAF Score for " + codecName + " encoding in function of CRF\"" << std::endl;
					vmafPlotFile << "set xlabel \"CRF\"" << std::endl;
					vmafPlotFile << "set ylabel \"VMAF Score\"" << std::endl;
					vmafPlotFile << "set key left bottom title \"Preset\" enhanced" << std::endl;
					vmafPlotFile << "plot ";
				}

				for (const auto& result: results) {
					if (generatePlotFile) {
						if (index != 2) {
							vmafPlotFile << ", ";
						}
						vmafPlotFile << "'vmaf-" + shortCodecName + ".dat' using 1:" + std::to_string(index++) + " with linespoints linewidth 1 title \"" + result.preset + "\"";
					}
					vmafFile << "\t" << result.vmaf;
				}

				if (generatePlotFile) {
					vmafPlotFile << std::endl;
					generatePlotFile = false;
				}

				vmafFile << std::endl;
			}
		}

		{
			std::ofstream bitstreamSizeFile("bitstream-" + shortCodecName + ".dat", std::ios::out);
			std::ofstream bitstreamPlotFile("bitstream-" + shortCodecName + "-plot.gpi", std::ios::out);

			assert(bitstreamSizeFile.good());
			assert(bitstreamPlotFile.good());

			bool generatePlotFile = true;
			int index = 2;
			for (auto& [key, results]: bitstreamSizeResults) {
				bitstreamSizeFile << key;

				std::sort(results.begin(), results.end(), [](const auto& lhs, const auto& rhs) {
					return lhs.preset < rhs.preset;
				});

				if (generatePlotFile) {
					bitstreamPlotFile << "set terminal pdf" << std::endl;
					bitstreamPlotFile << "set output \"bitstream-" + shortCodecName + ".pdf\"" << std::endl;
					bitstreamPlotFile << "set title \"Bitstream size for " + codecName + " encoding in function of CRF\"" << std::endl;
					bitstreamPlotFile << "set xlabel \"CRF\"" << std::endl;
					bitstreamPlotFile << "set ylabel \"Bitstream Size\"" << std::endl;
					bitstreamPlotFile << "set key right top title \"Preset\" enhanced" << std::endl;
					bitstreamPlotFile << "plot ";
				}

				for (const auto& result: results) {
					if (generatePlotFile) {
						if (index != 2) {
							bitstreamPlotFile << ", ";
						}
						bitstreamPlotFile << "'bitstream-" + shortCodecName + ".dat' using 1:" + std::to_string(index++) + " with linespoints linewidth 1 title \"" + result.preset + "\"";
					}
					bitstreamSizeFile << "\t" << result.bitstreamSize;
				}

				if (generatePlotFile) {
					bitstreamPlotFile << std::endl;
					generatePlotFile = false;
				}

				bitstreamSizeFile << std::endl;
			}
		}

		{
			std::ofstream encodingFile("encoding-" + shortCodecName + ".dat", std::ios::out);
			std::ofstream encodingPlotFile("encoding-" + shortCodecName + "-plot.gpi", std::ios::out);

			assert(encodingFile.good());
			assert(encodingPlotFile.good());

			bool generatePlotFile = true;
			int index = 2;
			for (auto& [key, results]: encodingTimeResults) {
				encodingFile << key;

				std::sort(results.begin(), results.end(), [](const auto& lhs, const auto& rhs) {
					return lhs.preset < rhs.preset;
				});

				if (generatePlotFile) {
					encodingPlotFile << "set terminal pdf" << std::endl;
					encodingPlotFile << "set output \"encoding-" + shortCodecName + ".pdf\"" << std::endl;
					encodingPlotFile << "set title \"Encoding time for " + codecName + " encoding in function of CRF\"" << std::endl;
					encodingPlotFile << "set xlabel \"CRF\"" << std::endl;
					encodingPlotFile << "set ylabel \"Encoding time\"" << std::endl;
					encodingPlotFile << "set key top right title \"Preset\" enhanced" << std::endl;
					encodingPlotFile << "plot ";
				}

				for (const auto& result: results) {
					if (generatePlotFile) {
						if (index != 2) {
							encodingPlotFile << ", \\" << std::endl;
						}
						encodingPlotFile << "'encoding-" + shortCodecName + ".dat' using 1:" + std::to_string(index++) + " with linespoints linewidth 1 title \"" + result.preset + "\"";
					}
					encodingFile << "\t" << result.encodingTime;
				}

				if (generatePlotFile) {
					encodingPlotFile << std::endl;
					generatePlotFile = false;
				}

				encodingFile << std::endl;
			}
		}
		{
			std::ofstream decodingFile("decoding-" + shortCodecName + ".dat", std::ios::out);
			std::ofstream decodingPlotFile("decoding-" + shortCodecName + "-plot.gpi", std::ios::out);

			assert(decodingFile.good());
			assert(decodingPlotFile.good());

			bool generatePlotFile = true;
			int index = 2;
			for (auto& [key, results]: decodingTimeResults) {
				decodingFile << key;

				std::sort(results.begin(), results.end(), [](const auto& lhs, const auto& rhs) {
					return lhs.preset < rhs.preset;
				});

				if (generatePlotFile) {
					decodingPlotFile << "set terminal pdf" << std::endl;
					decodingPlotFile << "set output \"decoding-" + shortCodecName + ".pdf\"" << std::endl;
					decodingPlotFile << "set title \"Decoding time for " + codecName + " encoding in function of CRF\"" << std::endl;
					decodingPlotFile << "set xlabel \"CRF\"" << std::endl;
					decodingPlotFile << "set ylabel \"Decoding time\"" << std::endl;
					decodingPlotFile << "set key top right title \"Preset\" enhanced" << std::endl;
					decodingPlotFile << "plot ";
				}

				for (const auto& result: results) {
					if (generatePlotFile) {
						if (index != 2) {
							decodingPlotFile << ", ";
						}
						decodingPlotFile << "'decoding-" + shortCodecName + ".dat' using 1:" + std::to_string(index++) + " with linespoints linewidth 1 title \"" + result.preset + "\"";
					}
					decodingFile << "\t" << result.decodingTime;
				}

				if (generatePlotFile) {
					decodingPlotFile << std::endl;
					generatePlotFile = false;
				}

				decodingFile << std::endl;
			}
		}
	}

	return 0;
}

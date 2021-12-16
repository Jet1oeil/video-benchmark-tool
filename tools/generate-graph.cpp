#include <cassert>

#include <iostream>
#include <fstream>

#include <json.hpp>

using json = nlohmann::json;

struct VMAFResult {
	std::string preset;
	double vmaf;
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
		std::string codecName;
		for (const auto& experiment: j["experiments"] ) {
			if (experiment["key"]["codec_id"] == codecID) {
				codecName = experiment["key"]["codec_name"];
				vmafResults[experiment["key"]["crf"]].push_back({ experiment["key"]["preset"], experiment["results"]["vmaf"] });
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

	return 0;
}

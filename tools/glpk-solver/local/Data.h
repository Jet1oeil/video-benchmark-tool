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

		bool operator<(const Configuration& other) const;
	};

	struct ExperimentResult {
		double dVMAF;
		int iEncodingTime;
		int iBitstreamSize;
	};

	struct ExperimentResultEntry {
		Configuration config;
		ExperimentResult result;
	};

	using ExperimentResults = std::vector<ExperimentResultEntry>;

	ExperimentResults loadExperimentResults(const std::filesystem::path& filePath);
}

#endif // LOCAL_DATA_H_

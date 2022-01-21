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

		std::string toString() const;
	};

	struct ExperimentResult {
		double dVMAF;
		int iEncodingTime;
		int iBitstreamSize;
	};

	struct ExperimentResultEntry {
		Configuration config;
		ExperimentResult result;

		bool operator<(const ExperimentResultEntry& other);
		bool operator==(const ExperimentResultEntry& other);
	};

	using ExperimentResults = std::vector<ExperimentResultEntry>;

	ExperimentResults loadExperimentResults(const std::filesystem::path& filePath);
	void fixedVMAFPareto(ExperimentResults resultsCopy, double vmafLimit);
}

#endif // LOCAL_DATA_H_

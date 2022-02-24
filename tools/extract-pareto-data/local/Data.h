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
	void fixedVMAFPareto(ExperimentResults resultsCopy, double limit);
	void fixedEncodingTime(ExperimentResults resultsCopy, double limit);
	void fixedBitstreamsize(ExperimentResults resultsCopy, double limit);
}

#endif // LOCAL_DATA_H_

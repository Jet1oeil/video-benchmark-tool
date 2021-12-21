#ifndef LOCAL_DATA_H_
#define LOCAL_DATA_H_

#include <filesystem>
#include <map>
#include <string>

namespace local {
	struct Configuration {
		std::string szCodecName;
		std::string szPreset;
		int iCRF;

		bool operator<(const Configuration& other) const;
	};

	struct Results {
		double dVMAF;
		int iEncodingTime;
		int iBitstreamSize;
	};

	using ResultsMap = std::map<Configuration, Results>;

	ResultsMap loadResults(const std::filesystem::path& filePath);
}

#endif // LOCAL_DATA_H_

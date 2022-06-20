#include "Results.h"

#include <fstream>

#include <json.hpp>

#include "Helper/Log.h"

using json = nlohmann::json;

namespace vmaf {

	namespace {
		json generateResultJSON(const Configuration& key, const Results& value) {
			helper::Log::debug("[Codec=%d, CRF=%d, preset=%s, bitrate=%d]: VMAF=%f", static_cast<int>(key.codecType), key.iCRF, key.szPreset.c_str(), key.iBitrate, value.dVMAFScore);

			json jKey = {
				"key", {
					{ "codec_id", static_cast<int>(key.codecType) },
					{ "codec_name", types::getCodecName(key.codecType) },
					{ "crf", key.iCRF },
					{ "preset", key.szPreset.c_str() },
					{ "bitrate", key.iBitrate }
				}
			};

			json jResult = {
				"results", {
					{ "vmaf", value.dVMAFScore },
					{ "bitstream_size", value.iBitstreamSize },
					{ "encoding_time", value.iEncodingTime },
					{ "decoding_time", value.iDecodingTime },
				}
			};

			return {jKey, jResult};
		}

		json generateJSON(const std::map<Configuration, Results>& results) {
			json jDocument;

			for (const auto& [configuration, result]: results) {
				jDocument["experiments"].push_back(
					generateResultJSON(configuration, result)
				);
			}

			return jDocument;
		}

		json generateJSON(const Configuration& configuration, const Results& results) {
			json jDocument;

			jDocument["experiments"].push_back(
				generateResultJSON(configuration, results)
			);

			return jDocument;
		}

		void dumpToHardDrive(const std::string& file, const json& jDocument )
		{
			std::ofstream jsonFile;

			jsonFile.open(file);
			if (!jsonFile.good()) {
				helper::Log::error("Unable to store results :\n%s", jDocument.dump(4).c_str());
				return;
			}

			jsonFile << jDocument;
		}
	}

	void writeResult(const std::string& resultFile, const std::map<Configuration, Results>& results)
	{
		json jDocument = generateJSON(results);
		dumpToHardDrive(resultFile, jDocument);
	}

	void writeResult(const std::string& resultFile, const Configuration& configuration, const Results& results)
	{
		json jDocument = generateJSON(configuration, results);
		dumpToHardDrive(resultFile, jDocument);
	}

}

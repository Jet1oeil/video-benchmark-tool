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
					{ "encoding_time", value.dEncodingTime },
					{ "decoding_time", value.dDecdodingTime },
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
	}

	void writeResult(const std::string& resultFile, const std::map<Configuration, Results>& results)
	{
		json jDocument = generateJSON(results);

		std::ofstream jsonFile;

		jsonFile.open(resultFile);
		if (!jsonFile.good()) {
			helper::Log::error("Unable to store results :\n%s", jDocument.dump(4).c_str());
			return;
		}

		jsonFile << jDocument;

		return;
	}

}

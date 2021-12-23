#include "Benchmark.h"

#include <clocale>
#include <fstream>
#include <mutex>

#include <QDateTime>
#include <QLocale>

#include <json.hpp>

#include "Process/helper/AVCodecHelper.h"
#include "Process/helper/AVFormatHelper.h"

#include "Configuration.h"

using json = nlohmann::json;

namespace vmaf {
	void Benchmark::start(const QString& szVideoFileName, const CodecList& listCodec, int iMinCRF, int iMaxCRF, const QStringList& listPreset)
	{
		qDebug("selected video: %s", qPrintable(szVideoFileName));
		qDebug("selected codec:");
		for (const auto& codecID: listCodec) {
			qDebug("\t%d", static_cast<int>(codecID));
		}
		qDebug("CRF: [%d - %d]", iMinCRF, iMaxCRF);
		qDebug("selected preset:");
		for (const auto& szPreset: listPreset) {
			qDebug("\t%s", qPrintable(szPreset));
		}

		types::PacketList yuvFrames;
		if (!decodeOriginalVideoFile(szVideoFileName, yuvFrames)) {
			qDebug("Error decoding...");
		}

		runExperiments(yuvFrames, listCodec, iMinCRF, iMaxCRF, listPreset);
	}

	bool Benchmark::decodeOriginalVideoFile(const QString& szVideoFileName, types::PacketList& yuvFrames)
	{
		helper::avcodec::Context codecContex;

		if (codecContex.decodeVideoFile(qPrintable(szVideoFileName), yuvFrames) != helper::avcodec::Error::Success) {
			qDebug("Error decode video...");
			return false;
		}

		m_originalCodecParameters = codecContex.getCodecParameters();

		return true;
	}

	void Benchmark::runExperiments(const types::PacketList& yuvFrames, const CodecList& listCodec, int iMinCRF, int iMaxCRF, const QStringList& listPreset)
	{
		std::vector<Configuration> listConfigurations;

		// Generate all configuration
		for (const auto& codecID: listCodec) {
			for (int iCRF = iMinCRF; iCRF <= iMaxCRF; ++iCRF) {
				for (const auto& szPreset: listPreset) {
					listConfigurations.push_back({ codecID, iCRF, szPreset.toStdString() });
				}
			}
		}

		// Keep previous locale
		std::string szCurrentLocale = std::setlocale(LC_NUMERIC, nullptr);

		// Set locale to 'C' to avoid bug during vmaf model loading
		// Must be done before threads launch since it's not thread-safe
		std::setlocale(LC_NUMERIC, "C");

		// Alloc the thread pool
		std::mutex mutexExperiments;
		for (int i = 0; i < static_cast<int>(std::thread::hardware_concurrency()); ++i) {
			m_poolThreads.emplace_back(yuvFrames, m_originalCodecParameters, listConfigurations, mutexExperiments);
		}

		// Start all threads
		for (auto& thread: m_poolThreads) {
			thread.start();
		}

		// Join all thread
		for (auto& thread: m_poolThreads) {
			thread.wait();
			auto threadResults = thread.getResults();
			m_results.insert(threadResults.begin(), threadResults.end());
		}

		// Restore the previous locale
		std::setlocale(LC_NUMERIC, szCurrentLocale.c_str());

		// Print results
		json jsonDocument;
		for (const auto& [key, value]: m_results) {
			qDebug("[Codec=%d, CRF=%d, preset=%s]: VMAF=%f", static_cast<int>(key.codecType), key.iCRF, key.szPreset.c_str(), value.dVMAFScore);
			json jKey = {
				"key", {
					{ "codec_id", static_cast<int>(key.codecType) },
					{ "codec_name", types::getCodecName(key.codecType) },
					{ "crf", key.iCRF },
					{ "preset", key.szPreset.c_str() }
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
			jsonDocument["experiments"].push_back(
				{ jKey, jResult }
			);
		}

		std::ofstream jsonFile;

		QLocale enLocale = QLocale(QLocale::English, QLocale::UnitedStates);
		QString dateTimeText = enLocale.toString(QDateTime::currentDateTime(), "yyyy-MM-dd-HHmmss");
		jsonFile.open(dateTimeText.toStdString() + "-results.json");

		if (!jsonFile.good()) {
			qCritical("Unable to store results :\n%s", jsonDocument.dump(4).c_str());
			return;
		}

		jsonFile << jsonDocument;
	}
}

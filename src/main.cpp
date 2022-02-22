#include <iostream>

#include <QApplication>

#include "CLI/OptionParser.h"

#include "Controller/QBenchmarkController.h"

#include "Process/vmaf/Benchmark.h"

#include "Types/Codec.h"

#include "View/QMainView.h"

int run_cli(int argc, char *argv[]);
int run_gui(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	bool bCLI = false;

	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];

		if (arg == "--cli") {
			bCLI = true;
			break;
		}
	}

	if (bCLI) {
		return run_cli(argc, argv);
	}

	return run_gui(argc, argv);
}

int run_cli(int argc, char *argv[])
{
	cli::OptionParser parser;
	parser.registerOption("cli");
	parser.registerOption("video-source", "no-defined");
	parser.registerOption("codec-list", "all");
	parser.registerOption("crf", "0-51");
	parser.registerOption("preset-list", "all");

	parser.parse(argc, argv);

	auto printUsage = [argv](const std::string& err) {
		std::cerr << err << std::endl;
		std::cerr << "Usage:" << std::endl;
		std::cerr << "    " << argv[0] << " [OPTION]" << std::endl;
		std::cerr << "CLI option:" << std::endl;
		std::cerr << "    --cli: use cli interface" << std::endl;
		std::cerr << "    --video-source VIDEO_PATH: set the video source (mandatory)" << std::endl;
		std::cerr << "    --crf DOWN-UP | --crf VAL: define the crf level (between 0-51)" << std::endl;
		std::cerr << "    --codec-list LIST: set selected codecs separated by comma (default: all)" << std::endl;
		std::cerr << "        Available codec:" << std::endl;
		std::cerr << "            - all" << std::endl;
		for (auto& codec: types::CodecList) {
			std::cerr << "            - " << codec << std::endl;
		}
		std::cerr << "    --preset-list LIST: set selected presets separated by comma (default: all)" << std::endl;
		std::cerr << "        Available codec:" << std::endl;
		std::cerr << "            - all" << std::endl;
		for (auto& preset: types::PresetList) {
			std::cerr << "            - " << preset << std::endl;
		}
	};

	if (!parser["video-source"]) {
		printUsage("A video source must be defined");

		return 1;
	}

	// Store video source
	const auto& video = parser["video-source"].value;

	// Split string function
	auto splitString = [](std::string str, const std::string& delimiter = ",") {
		std::vector<std::string> list;
		size_t pos = 0;
		std::string token;

		while ((pos = str.find(delimiter)) != std::string::npos) {
			token = str.substr(0, pos);
			list.push_back(token);
			str.erase(0, pos + delimiter.length());
		}
		list.push_back(str);

		return list;
	};

	// Get CRF bounds
	int minCRF = -1;
	int maxCRF = -1;
	const auto& crfArg = parser["crf"].value;
	auto itDelimiter = std::find_if(crfArg.begin(), crfArg.end(), [](auto c){
		return c == '-';
	});
	if (itDelimiter == crfArg.end()) {
		minCRF = std::stoi(crfArg);
		maxCRF = minCRF;
	} else {
		auto listCRF = splitString(crfArg, "-");

		if (listCRF.size() != 2) {
			printUsage("Invalid CRF bounds");
			return 1;
		}

		minCRF = std::stoi(listCRF[0]);
		maxCRF = std::stoi(listCRF[1]);

		if (minCRF > maxCRF) {
			std::swap(minCRF, maxCRF);
		}
	}

	if ((minCRF < 0 || minCRF > 51) || (maxCRF < 0 || maxCRF > 51) || (minCRF > maxCRF)) {
		printUsage("Invalid CRF bounds");
		return 1;
	}

	// Create codec list
	CodecList listCodec;

	if (parser["codec-list"].value == "all") {
		for (const auto& codec: types::CodecList) {
			listCodec.push_back(types::getCodecID(codec));
		}
	} else {
		const auto& argCodecList = parser["codec-list"].value;

		auto listCodecString = splitString(argCodecList);

		// Check if all codec are valid
		for (auto selectedCodec: listCodecString) {
			auto it = std::find(types::CodecList.begin(), types::CodecList.end(), selectedCodec);
			if (it == types::CodecList.end()) {
				printUsage("Invalid codec '" + selectedCodec + "'");
				return 1;
			}

			listCodec.push_back(types::getCodecID(selectedCodec));
		}
	}

	// Create preset list
	std::vector<std::string> listPreset;

	if (parser["preset-list"].value == "all") {
		listPreset.insert(listPreset.end(), types::PresetList.begin(), types::PresetList.end());
	} else {
		const auto& argPrestList = parser["preset-list"].value;

		listPreset = splitString(argPrestList);

		// Check if all preset are valid
		for (auto selectedPreset: listPreset) {
			auto it = std::find(types::PresetList.begin(), types::PresetList.end(), selectedPreset);
			if (it == types::PresetList.end()) {
				printUsage("Invalid preset '" + selectedPreset + "'");
				return 1;
			}
		}
	}

	// Run VMAF benchmark
	vmaf::Benchmark benchmark;

	benchmark.start(video, listCodec, minCRF, maxCRF, listPreset);

	return 0;
}

int run_gui(int argc, char *argv[])
{
	QApplication application(argc, argv);

	// Views
	view::QMainView mainView;
	mainView.show();

	// Controllers
	ctrl::QBenchmarkController benchmarkConfigController(mainView);

	return application.exec();
}

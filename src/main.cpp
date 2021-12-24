#include <iostream>

#include <QApplication>

#include "CLI/OptionParser.h"

#include "Controller/QBenchmarkConfigController.h"

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

	return 0;
}

int run_gui(int argc, char *argv[])
{
	QApplication application(argc, argv);

	// Views
	view::QMainView mainView;
	mainView.show();

	// Controllers
	ctrl::QBenchmarkConfigController benchmarkConfigController(mainView);

	return application.exec();
}

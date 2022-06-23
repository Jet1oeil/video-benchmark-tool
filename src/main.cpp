/* Copyright (c) 2022 Jet1oeil
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <iostream>

#include <QApplication>

#include "CLI/OptionParser.h"

#include "Controller/QBenchmarkController.h"

#include "Helper/Log.h"

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
	parser.registerOption("resume");
	parser.registerOption("video-source", "no-defined");
	parser.registerOption("config", "no-defined");

	parser.parse(argc, argv);

	auto printUsage = [argv](const std::string& err) {
		std::cerr << err << std::endl;
		std::cerr << "Usage:" << std::endl;
		std::cerr << "    " << argv[0] << " [OPTION]" << std::endl;
		std::cerr << "CLI option:" << std::endl;
		std::cerr << "    --cli: use cli interface" << std::endl;
		std::cerr << "    --resume: start the experimentation from the last configuration (other parameters will be ignored)" << std::endl;
		std::cerr << "    --video-source VIDEO_PATH: set the video source (mandatory)" << std::endl;
		std::cerr << "    --config JSON_CONFIG_FILE_PATH: define the json config file to setup encoder configurations, if omitted all configurations will be selected" << std::endl;
	};

	// Resume from the last experiment
	if (parser["resume"]) {
		// Run VMAF benchmark
		vmaf::Benchmark benchmark;

		benchmark.resume();

		return 0;
	}

	if (!parser["video-source"]) {
		printUsage("A video source must be defined");

		return 1;
	}

	// Store video source
	const auto& video = parser["video-source"].value;
	helper::Log::info("Selected video source: %s", video.c_str());
	helper::Log::info(" ");

	std::vector<vmaf::Configuration> listConfigurations;
	if (!parser["config"]) {
		listConfigurations = vmaf::generateAllConfigurations();
	} else {
		listConfigurations = vmaf::generateConfigurationsFromJSON(parser["config"].value);
	}

	// Run VMAF benchmark
	vmaf::Benchmark benchmark;

	benchmark.start(video, listConfigurations);

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

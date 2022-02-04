#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>

#include "local/Data.h"

int main(int argc, char* argv[])
{
	if (argc != 2) {
		std::cerr << "Missing parameters" << std::endl;
		std::cerr << "Usage:" << std::endl;
		std::cerr << "\t" << argv[0] << " RESULTS_JSON_FILE" << std::endl;

		return 1;
	}

	auto exps = local::loadExperimentResults(argv[1]);
	assert(exps.size() > 0);

	for (int i = 70; i <= 95; i += 5) {
		local::fixedVMAFPareto(exps, i);
	}

	for (int i = 4000; i <= 16000; i += 1000) {
		local::fixedEncodingTime(exps, i);
	}

	constexpr int mega = 1024*1024;
	for (double i = 0.7 * mega ; i < 2.0 * mega; i += 0.1 * mega) {
		local::fixedBitstreamsize(exps, std::round(i));
	}

	return 0;
}
#include <cassert>
#include <iostream>

#include "local/Data.h"

int main(int argc, char* argv[])
{
	if (argc != 4) {
		std::cerr << "Missing parameters" << std::endl;
		std::cerr << "Usage:" << std::endl;
		std::cerr << "\t" << argv[0] << " RESULTS_JSON_FILE ORIGINAL_SIZE ORIGINAL_DURATION" << std::endl;

		return 1;
	}

	int originalSize = std::stoi(argv[2]); // octets
	int originalDuration = std::stoi(argv[3]) * 1000; // expect seconds convert to ms

	auto exps = local::loadExperimentResults(argv[1], originalSize, originalDuration);
	assert(exps.size() > 0);

	for (int i = 70; i <= 95; i += 5) {
		local::fixedVMAFPareto(exps, i);
	}

	for (double i = 0.5; i <= 1.5; i += 0.1) {
		local::fixedEncodingTime(exps, i);
	}

	for (double i = 0.5; i <= 1.5; i += 0.1) {
		local::fixedBitstreamsize(exps, i);
	}

	return 0;
}

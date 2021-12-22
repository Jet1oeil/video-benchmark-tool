#include <cassert>
#include <iostream>

#include "local/Data.h"
#include "local/LinearSolver.h"

int main(int argc, char* argv[])
{
	if (argc != 3) {
		std::cerr << "Missing parameters" << std::endl;
		std::cerr << "Usage:" << std::endl;
		std::cerr << "\t" << argv[0] << " RESULTS_JSON_FILE TMAX_CONSTANT" << std::endl;

		return 1;
	}

	auto results = local::loadResults(argv[1]);
	assert(results.size() > 0);

	local::LinearSolver solver(std::stod(argv[2]));
	solver.fillContraints(results);
	int iResultIndex = solver.solve();

	if (iResultIndex == -1) {
		return 1;
	}

	std::cout << "Selected config: " << std::endl;
	std::cout << "\t- codec: " << results[iResultIndex].config.szCodecName << std::endl;
	std::cout << "\t- preset: " << results[iResultIndex].config.szPreset << std::endl;
	std::cout << "\t- crf: " << results[iResultIndex].config.iCRF << std::endl;
	std::cout << "\t- vmaf: " << results[iResultIndex].result.dVMAF << std::endl;
	std::cout << "\t- encoding time: " << results[iResultIndex].result.iEncodingTime << std::endl;

	return 0;
}

#include <cassert>
#include <iostream>

#include "local/Data.h"
#include "local/LinearSolver.h"

bool solveLinearProgram(const local::ExperimentResults& exps, double vmafLimit, double maxTime)
{
	local::LinearSolver solver(vmafLimit, maxTime);
	solver.fillContraints(exps);
	int iResultIndex = solver.solve();

	if (iResultIndex == -1) {
		return false;
	}

	std::cout << "Selected config: " << std::endl;
	std::cout << "\t- codec: " << exps[iResultIndex].config.szCodecName << std::endl;
	std::cout << "\t- preset: " << exps[iResultIndex].config.szPreset << std::endl;
	std::cout << "\t- crf: " << exps[iResultIndex].config.iCRF << std::endl;
	std::cout << "\t- vmaf: " << exps[iResultIndex].result.dVMAF << std::endl;
	std::cout << "\t- encoding time: " << exps[iResultIndex].result.iEncodingTime << " ms" << std::endl;

	return true;
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		std::cerr << "Missing parameters" << std::endl;
		std::cerr << "Usage:" << std::endl;
		std::cerr << "\t" << argv[0] << " RESULTS_JSON_FILE TMAX_CONSTANT" << std::endl;

		return 1;
	}

	auto exps = local::loadExperimentResults(argv[1]);
	assert(exps.size() > 0);

	for (int i = 50; i < 100; ++i) {
		solveLinearProgram(exps, i, std::stod(argv[2]));
	}

	return 0;
}

#include <cassert>
#include <iostream>
#include <fstream>

#include "local/Data.h"
#include "local/LinearSolver.h"

bool solveLinearProgram(const local::ExperimentResults& exps, double vmafLimit, double maxTime, std::ofstream& solFile, std::ofstream& dataFile)
{
	local::LinearSolver solver(vmafLimit, maxTime);
	solver.fillContraints(exps);
	int iResultIndex = solver.solve();

	if (iResultIndex == -1) {
		return false;
	}

	solFile << vmafLimit;
	solFile << "\t\t" << maxTime;
	solFile << "\t\t" << exps[iResultIndex].config.szCodecName;
	solFile << "\t"<< exps[iResultIndex].config.szPreset;
	solFile << "\t" << exps[iResultIndex].config.iCRF;
	solFile << "\t" << exps[iResultIndex].result.dVMAF;
	solFile << "\t" << exps[iResultIndex].result.iEncodingTime << std::endl;

	dataFile << vmafLimit;
	dataFile << "\t" << exps[iResultIndex].result.iEncodingTime;
	dataFile << "\t" << exps[iResultIndex].result.iBitstreamSize;
	dataFile << "\t" << exps[iResultIndex].result.dVMAF << std::endl;

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

	// Iterate over VMAF limit
	std::ofstream vmafSolFile("vmaf-time-1.0.sol", std::ios::out);
	assert(vmafSolFile.good());
	vmafSolFile << "vmaf limit\tmax time\tcodec\t\tpreset\t\tcrf\tvmaf\tencoding time" << std::endl;

	std::ofstream vmafDataFile("vmaf-time-1.0.dat", std::ios::out);
	assert(vmafDataFile.good());

	for (int i = 50; i < 100; ++i) {
		solveLinearProgram(exps, i, std::stod(argv[2]), vmafSolFile, vmafDataFile);
	}

	return 0;
}

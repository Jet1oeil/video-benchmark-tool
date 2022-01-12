#include <cassert>
#include <cmath>
#include <iomanip>
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
	solFile << "\t" << exps[iResultIndex].result.iEncodingTime;
	solFile << "\t" << exps[iResultIndex].result.iBitstreamSize << std::endl;

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
	double cmdTimeLimit = std::stod(argv[2]) * 1000.0;

	for (double timeLimit = std::round(cmdTimeLimit * 0.5); timeLimit <= std::round(cmdTimeLimit * 1.5); timeLimit += std::round(cmdTimeLimit * 0.1)) {
		std::ostringstream paddingTimeLimit;
		paddingTimeLimit << std::setw(5) << std::setfill('0') << static_cast<int>(timeLimit);

		std::ofstream vmafSolFile("vmaf-time-limit-" + paddingTimeLimit.str() + ".sol", std::ios::out);
		assert(vmafSolFile.good());
		vmafSolFile << "vmaf limit\tmax time\tcodec\t\tpreset\t\tcrf\tvmaf\tencoding time\tbitstream size" << std::endl;

		std::ofstream vmafDataFile("vmaf-time-limit-" + paddingTimeLimit.str() + ".dat", std::ios::out);
		assert(vmafDataFile.good());

		for (int i = 50; i < 100; ++i) {
			solveLinearProgram(exps, i, timeLimit, vmafSolFile, vmafDataFile);
		}
	}

	// Debug purpose: only process Tmax = 1000
	// std::ofstream vmafDataFile("vmaf-time-limit-1000.dat", std::ios::out);
	// std::ofstream vmafSolFile("vmaf-time-limit-1000.sol", std::ios::out);
	// for (int i = 50; i < 100; ++i) {
	// 	solveLinearProgram(exps, i, cmdTimeLimit, vmafSolFile, vmafDataFile);
	// }

	return 0;
}

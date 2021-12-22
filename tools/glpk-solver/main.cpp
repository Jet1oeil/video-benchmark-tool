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
	solver.solve();

	return 0;
}

#include <iostream>

#include "local/LinearSolver.h"

int main(int argc, char* argv[])
{
	if (argc != 2) {
		std::cerr << "Missing parameter" << std::endl;
		std::cerr << "Usage" << std::endl;
		std::cerr << "\t" << argv[0] << " TMAX_CONSTANT" << std::endl;

		return 1;
	}

	local::LinearSolver solver(std::stod(argv[1]));

	return 0;
}

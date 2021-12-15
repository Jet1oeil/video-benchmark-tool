#include <iostream>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Missing parameters" << std::endl;
		std::cerr << "Usage:" << std::endl;
		std::cerr << "\t" << argv[0] << " RESULT_FILE" << std::endl;
		return 1;
	}

	return 0;
}

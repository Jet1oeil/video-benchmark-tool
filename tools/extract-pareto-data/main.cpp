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

	for (double i = 0.1; i < 1.7; i += 0.1) {
		local::fixedEncodingTime(exps, i);
	}

	for (double i = 0.01; i < 0.051; i += 0.002) {
		local::fixedBitstreamsize(exps, i);
	}

	separatedCodec(exps);

	return 0;
}

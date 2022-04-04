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

#ifndef LOCAL_LINEAR_SOLVER_H_
#define LOCAL_LINEAR_SOLVER_H_

#include <utility>

#include "Data.h"

extern "C" {
	struct glp_prob;
}

namespace local {
	class LinearSolver {
	public:
		LinearSolver(double dVMAFLimit, double dVideoDuration);
		~LinearSolver();

		LinearSolver(const LinearSolver&) = delete;
		LinearSolver(LinearSolver&&) = delete;

		LinearSolver& operator=(const LinearSolver&) = delete;
		LinearSolver& operator=(LinearSolver&&) = delete;

		void fillContraints(const ExperimentResults& results);
		int solve();

	private:
		glp_prob* m_pProgram;
		std::size_t m_iTotalVariables;
		double m_dVideoDuration;
	};
}

#endif // LOCAL_LINEAR_SOLVER_H_

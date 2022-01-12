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

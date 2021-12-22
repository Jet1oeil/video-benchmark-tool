#ifndef LOCAL_LINEAR_SOLVER_H_
#define LOCAL_LINEAR_SOLVER_H_

#include "Data.h"

extern "C" {
	struct glp_prob;
}

namespace local {
	class LinearSolver {
	public:
		LinearSolver(double dVideoDuration);
		~LinearSolver();

		LinearSolver(const LinearSolver&) = delete;
		LinearSolver(LinearSolver&&) = delete;

		LinearSolver& operator=(const LinearSolver&) = delete;
		LinearSolver& operator=(LinearSolver&&) = delete;

		void fillContraints(const ResultsMap& results);

	private:
		glp_prob* m_pProgram;
	};
}

#endif // LOCAL_LINEAR_SOLVER_H_

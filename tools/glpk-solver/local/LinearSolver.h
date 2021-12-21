#ifndef LOCAL_LINEAR_SOLVER_H_
#define LOCAL_LINEAR_SOLVER_H_

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

	private:
		glp_prob* m_pProgram;
	};
}

#endif // LOCAL_LINEAR_SOLVER_H_

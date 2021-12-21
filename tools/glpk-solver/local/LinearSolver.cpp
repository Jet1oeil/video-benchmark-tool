#include "LinearSolver.h"

#include <glpk.h>

namespace local {
	LinearSolver::LinearSolver(double dVideoDuration)
	: m_pProgram(glp_create_prob())
	{
		// Define linear program
		glp_set_prob_name(m_pProgram, "best-bitstream-size");

		// Define objective row
		glp_set_obj_dir(m_pProgram, GLP_MIN);
		glp_set_obj_name(m_pProgram, "min-bitstream-size");

		// Create constraint rows
		int iRow = glp_add_rows(m_pProgram, 4);

		// Only one config
		glp_set_row_name(m_pProgram, iRow, "delta");
		glp_set_row_bnds(m_pProgram, iRow, GLP_FX, 1.0, 0.0);

		// VMAF score over 90
		++iRow;
		glp_set_row_name(m_pProgram, iRow, "vmaf");
		glp_set_row_bnds(m_pProgram, iRow, GLP_UP, 0.0, 90.0);

		// Encoding time less than video duration
		++iRow;
		glp_set_row_name(m_pProgram, iRow, "encoding-time");
		glp_set_row_bnds(m_pProgram, iRow, GLP_UP, 0.0, dVideoDuration); // sum - Tmax >= 0

		// Bitstream size
		++iRow;
		glp_set_row_name(m_pProgram, iRow, "bitstream-size");
		glp_set_row_bnds(m_pProgram, iRow, GLP_FX, 0.0, 0.0); // sum - Btotal = 0
	}

	LinearSolver::~LinearSolver()
	{
		glp_delete_prob(m_pProgram);
		glp_free_env();
	}
}

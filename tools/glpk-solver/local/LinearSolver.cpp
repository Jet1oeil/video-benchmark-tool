#include "LinearSolver.h"

#include <cassert>
#include <iostream>

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

	void LinearSolver::fillContraints(const ResultsMap& results)
	{
		std::array<char, 128> buffer;

		// Define variables
		int iIndexCol = glp_add_cols(m_pProgram, results.size());

		for (std::size_t i = iIndexCol; i <= results.size(); ++i) {
			std::snprintf(buffer.data(), buffer.size(), "delta_%ld", (i - 1));

			glp_set_col_name(m_pProgram, i, buffer.data());
			glp_set_col_bnds(m_pProgram, i, GLP_LO, 0.0, 0.0);
			glp_set_col_kind(m_pProgram, i, GLP_BV); // Binary variable
			glp_set_obj_coef(m_pProgram, i, 0.0);
		}

		// Add total size variable
		iIndexCol = glp_add_cols(m_pProgram, 1);
		glp_set_col_name(m_pProgram, iIndexCol, "B_max");
		glp_set_col_bnds(m_pProgram, iIndexCol, GLP_LO, 0.0, 0.0);
		glp_set_obj_coef(m_pProgram, iIndexCol, -1.0);

		std::vector<int> indexRowArray;
		std::vector<int> indexColArray;
		std::vector<double> coeffArray;

		// unused 0 index
		indexRowArray.push_back(0);
		indexColArray.push_back(0);
		coeffArray.push_back(0.0);

		// delta_i coefficients
		for (std::size_t i = 0; i < results.size(); ++i) {
			int iCol = i + 1;
			indexRowArray.push_back(1);
			indexColArray.push_back(iCol);
			coeffArray.push_back(1.0);
		}

		// vmaf coeffcients
		int iCol = 1;
		for (const auto& result: results) {
			indexRowArray.push_back(2);
			indexColArray.push_back(iCol++);
			coeffArray.push_back(result.second.dVMAF);
		}

		// Encoding coeffcients
		iCol = 1;
		for (const auto& result: results) {
			indexRowArray.push_back(3);
			indexColArray.push_back(iCol++);
			coeffArray.push_back(result.second.iEncodingTime);
		}

		// Bitstreams size coeffcients
		iCol = 1;
		for (const auto& result: results) {
			indexRowArray.push_back(4);
			indexColArray.push_back(iCol++);
			coeffArray.push_back(result.second.iBitstreamSize);
		}

		// Define total size variable
		indexRowArray.push_back(4);
		indexColArray.push_back(iCol);
		coeffArray.push_back(-1.0);

		assert(indexRowArray.size() == indexColArray.size());
		assert(indexRowArray.size() == coeffArray.size());

		glp_load_matrix(m_pProgram, indexRowArray.size() - 1, &indexRowArray[0], &indexColArray[0], &coeffArray[0]);
	}
}

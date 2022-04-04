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

#include "LinearSolver.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <iostream>

#include <glpk.h>

namespace fs = std::filesystem;

namespace local {
	LinearSolver::LinearSolver(double dVMAFLimit, double dVideoDuration)
	: m_pProgram(glp_create_prob())
	, m_iTotalVariables(0)
	, m_dVideoDuration(dVideoDuration)
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
		glp_set_row_bnds(m_pProgram, iRow, GLP_LO, dVMAFLimit, 0.0);

		// Encoding time less than video duration
		++iRow;
		glp_set_row_name(m_pProgram, iRow, "encoding-time");
		glp_set_row_bnds(m_pProgram, iRow, GLP_FX, 0.0, 0.0); // sum - Ttotal = 0

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

	void LinearSolver::fillContraints(const ExperimentResults& results)
	{
		std::array<char, 128> buffer;

		// Define variables
		m_iTotalVariables = results.size();
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
		glp_set_obj_coef(m_pProgram, iIndexCol, 1.0);

		// Add total size variable
		iIndexCol = glp_add_cols(m_pProgram, 1);
		glp_set_col_name(m_pProgram, iIndexCol, "T_total");
		glp_set_col_bnds(m_pProgram, iIndexCol, GLP_UP, 0.0, m_dVideoDuration);
		glp_set_obj_coef(m_pProgram, iIndexCol, 100.0);

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
			coeffArray.push_back(result.result.dVMAF);
		}

		// Encoding coeffcients
		iCol = 1;
		for (const auto& result: results) {
			indexRowArray.push_back(3);
			indexColArray.push_back(iCol++);
			coeffArray.push_back(result.result.iEncodingTime);
		}

		// Define total size variable
		indexRowArray.push_back(3);
		indexColArray.push_back(iCol + 1);
		coeffArray.push_back(-1.0);

		// Bitstreams size coeffcients
		iCol = 1;
		for (const auto& result: results) {
			indexRowArray.push_back(4);
			indexColArray.push_back(iCol++);
			coeffArray.push_back(result.result.iBitstreamSize);
		}

		// Define total size variable
		indexRowArray.push_back(4);
		indexColArray.push_back(iCol);
		coeffArray.push_back(-1.0);

		assert(indexRowArray.size() == indexColArray.size());
		assert(indexRowArray.size() == coeffArray.size());

		glp_load_matrix(m_pProgram, indexRowArray.size() - 1, &indexRowArray[0], &indexColArray[0], &coeffArray[0]);

#ifdef GLPK_DEBUG
		auto tmpDir = fs::temp_directory_path();

		std::string filename = tmpDir / "lp.txt";
		glp_write_lp(m_pProgram, 0, filename.c_str());
#endif
	}

	int LinearSolver::solve()
	{
		glp_iocp parm;
		glp_init_iocp(&parm);
		parm.presolve = GLP_ON;
		if (glp_intopt(m_pProgram, &parm) != 0) {
			std::cerr << "MIP program failed" << std::endl;
			return -1;
		}

		// Print result
		double dMinSize = glp_mip_obj_val(m_pProgram);
		std::cout << "bitstream size: " << dMinSize << " o" << std::endl;

		// Selected variable
		int iSelectedConfig = -1;
		for (std::size_t i = 1; i <= m_iTotalVariables; ++i) {
			double val = glp_mip_col_val(m_pProgram, i);
			if (val != 0.0) {
				assert(iSelectedConfig == -1);
				iSelectedConfig = i - 1;
			}
		}

		assert(iSelectedConfig != -1);

		return iSelectedConfig;
	}
}

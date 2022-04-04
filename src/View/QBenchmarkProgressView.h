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

#ifndef Q_BENCHMARK_PROGRESS_VIEW_H_
#define Q_BENCHMARK_PROGRESS_VIEW_H_

#include <QLabel>
#include <QMutex>
#include <QPushButton>
#include <QProgressBar>

namespace view {

	class QBenchmarkProgressView: public QWidget {
	Q_OBJECT

	public:
		QBenchmarkProgressView(QWidget* pParent);

		void setTotalExperiment(int iTotalExperiment);

		void showCancelAnimation();

	signals:
		void cancelBenchmarkTriggered();

	public slots:
		void updateProgress();

	private:
		QLabel* m_pLabel;
		QProgressBar* m_pProgressBar;
		QPushButton* m_pCancelButton;
		QMutex m_mutex;
	};

}

#endif // Q_BENCHMARK_PROGRESS_VIEW_H_

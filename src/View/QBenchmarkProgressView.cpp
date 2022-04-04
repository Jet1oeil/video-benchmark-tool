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

#include "QBenchmarkProgressView.h"

#include <QMutexLocker>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace view {
	QBenchmarkProgressView::QBenchmarkProgressView(QWidget* pParent)
	: QWidget(pParent)
	, m_pLabel(new QLabel(tr("Benchmark in progress..."), this))
	, m_pProgressBar(new QProgressBar(this))
	, m_pCancelButton(new QPushButton(tr("Cancel"), this))
	{
		QVBoxLayout* pLayout = new QVBoxLayout;

		pLayout->addStretch();

		// Centering label
		QHBoxLayout* pCenteringLabelLayout = new QHBoxLayout;
		pCenteringLabelLayout->addStretch();
		pCenteringLabelLayout->addWidget(m_pLabel);
		pCenteringLabelLayout->addStretch();
		pLayout->addLayout(pCenteringLabelLayout);

		// Add progress bar
		pLayout->addWidget(m_pProgressBar);

		// Centering cancel button
		QHBoxLayout* pCenteringButtonLayout = new QHBoxLayout;
		pCenteringButtonLayout->addStretch();
		pCenteringButtonLayout->addWidget(m_pCancelButton);
		pCenteringButtonLayout->addStretch();
		pLayout->addLayout(pCenteringButtonLayout);

		pLayout->addStretch();

		setLayout(pLayout);

		// Connect signals
		connect(m_pCancelButton, &QPushButton::clicked, this, &QBenchmarkProgressView::cancelBenchmarkTriggered);
	}

	void QBenchmarkProgressView::setTotalExperiment(int iTotalExperiment)
	{
		// Reset previous progress
		m_pProgressBar->reset();

		m_pProgressBar->setMinimum(0);
		m_pProgressBar->setMaximum(iTotalExperiment);
		m_pProgressBar->setValue(0);
	}

	void QBenchmarkProgressView::showCancelAnimation()
	{
		m_pLabel->setText(tr("Canceling benchmark..."));
		m_pCancelButton->setDisabled(true);
		m_pProgressBar->setDisabled(true);
	}

	void QBenchmarkProgressView::updateProgress()
	{
		if (!m_pProgressBar->isEnabled()) {
			return;
		}

		QMutexLocker lock(&m_mutex);

		int currentValue = m_pProgressBar->value();
		m_pProgressBar->setValue(currentValue + 1);
	}
}

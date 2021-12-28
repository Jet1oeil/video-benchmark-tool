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

		m_pProgressBar->setValue(42);

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
	}

	void QBenchmarkProgressView::setTotalExperiment(int iTotalExperiment)
	{
		// Reset previous progress
		m_pProgressBar->reset();

		m_pProgressBar->setMinimum(0);
		m_pProgressBar->setMaximum(iTotalExperiment);
		m_pProgressBar->setValue(0);
	}

	void QBenchmarkProgressView::updateProgress()
	{
		QMutexLocker lock(&m_mutex);

		int currentValue = m_pProgressBar->value();
		m_pProgressBar->setValue(currentValue + 1);
	}
}

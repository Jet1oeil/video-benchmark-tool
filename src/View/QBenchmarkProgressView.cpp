#include "QBenchmarkProgressView.h"

#include <QMutexLocker>
#include <QVBoxLayout>

namespace view {
	QBenchmarkProgressView::QBenchmarkProgressView(QWidget* pParent)
	: QWidget(pParent)
	, m_pProgressBar(new QProgressBar(this))
	{
		QVBoxLayout* pLayout = new QVBoxLayout;

		m_pProgressBar->setValue(42);

		pLayout->addStretch();
		pLayout->addWidget(m_pProgressBar);
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

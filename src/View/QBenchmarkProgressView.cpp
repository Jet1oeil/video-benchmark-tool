#include "QBenchmarkProgressView.h"

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
}

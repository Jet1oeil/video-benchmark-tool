#include "QMainView.h"

#include <QVBoxLayout>

#include "QBenchmarkConfigView.h"
#include "QBenchmarkProgressView.h"

namespace view {
	QMainView::QMainView()
	: m_pStackedWidget(new QStackedWidget(this))
	, m_pBenchmarkConfigView(new QBenchmarkConfigView(this))
	, m_pBenchmarkProgressView(new QBenchmarkProgressView(this))
	{
		QLayout* pLayout = new QVBoxLayout(this);

		m_pStackedWidget->addWidget(m_pBenchmarkConfigView);
		m_pStackedWidget->addWidget(m_pBenchmarkProgressView);

		pLayout->addWidget(m_pStackedWidget);
	}

	QBenchmarkConfigView& QMainView::getBenchmarkConfigView()
	{
		return *m_pBenchmarkConfigView;
	}

	void QMainView::showConfig()
	{
		m_pStackedWidget->setCurrentIndex(0);
	}

	void QMainView::showProgress()
	{
		m_pStackedWidget->setCurrentIndex(1);
	}
}

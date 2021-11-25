#include "QMainView.h"

#include <QVBoxLayout>

#include "QBenchmarkConfigView.h"

namespace view {
	QMainView::QMainView()
	: m_pBenchmarkConfigView(new QBenchmarkConfigView(this))
	{
		QLayout* pLayout = new QVBoxLayout(this);

		pLayout->addWidget(m_pBenchmarkConfigView);
	}
}

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

	QBenchmarkProgressView& QMainView::getBenchmarkProgressView()
	{
		return *m_pBenchmarkProgressView;
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

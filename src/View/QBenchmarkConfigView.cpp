#include "QBenchmarkConfigView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

namespace view {
	QBenchmarkConfigView::QBenchmarkConfigView(QWidget* pParent)
	: QWidget(pParent)
	, m_pFileLabel(new QLineEdit())
	, m_pFileButton(new QPushButton(tr("Browse")))
	, m_pMinCRFSpinBox(new QSpinBox())
	, m_pMaxCRFSpinBox(new QSpinBox())
	, m_pStartButton(new QPushButton(tr("Run benchmark")))
	{
		QVBoxLayout* pLayout = new QVBoxLayout(this);

		pLayout->addWidget(createFileGroupBox());
		pLayout->addStretch();
		pLayout->addWidget(createEncoderOptionsGroupBox());
		pLayout->addStretch();
		pLayout->addLayout(createControlButtons());

		// Connect signals
		connect(m_pFileButton, &QPushButton::clicked, this, &QBenchmarkConfigView::browseFileTriggered);
	}

	void QBenchmarkConfigView::setSelectedFile(const QString& szFileName)
	{
		m_pFileLabel->setText(szFileName);
	}

	QGroupBox* QBenchmarkConfigView::createFileGroupBox()
	{
		m_pFileLabel->setEnabled(false);
		m_pFileLabel->setPlaceholderText(tr("Select video source"));

		QGroupBox *pGroupBox = new QGroupBox(tr("Source video file"));

		QHBoxLayout* pLayout = new QHBoxLayout();
		pLayout->addWidget(m_pFileLabel);
		pLayout->addWidget(m_pFileButton);

		pGroupBox->setLayout(pLayout);

		return pGroupBox;
	}

	QGroupBox* QBenchmarkConfigView::createEncoderOptionsGroupBox()
	{
		QGroupBox *pGroupBox = new QGroupBox(tr("Codec configuration"));
		QVBoxLayout* pLayout = new QVBoxLayout();

		// CRF options
		QGroupBox *pCRFGroupBox = new QGroupBox(tr("CRF [0 - 51]"));
		QHBoxLayout* pCRFLayout = new QHBoxLayout();

		pCRFLayout->addWidget(new QLabel(tr("Minimal CRF")));
		m_pMinCRFSpinBox->setMinimum(0);
		m_pMinCRFSpinBox->setMaximum(51);
		m_pMinCRFSpinBox->setValue(0);
		pCRFLayout->addWidget(m_pMinCRFSpinBox);

		pCRFLayout->addWidget(new QLabel(tr("Maximal CRF")));
		m_pMaxCRFSpinBox->setMinimum(0);
		m_pMaxCRFSpinBox->setMaximum(51);
		m_pMaxCRFSpinBox->setValue(51);
		pCRFLayout->addWidget(m_pMaxCRFSpinBox);

		pCRFGroupBox->setLayout(pCRFLayout);

		// Preset selection
		m_listPresetCheckbox = {
			// No tr needed since it's libx264/265 implementation names
			new QCheckBox("ultrafast"),
			new QCheckBox("superfast"),
			new QCheckBox("veryfast"),
			new QCheckBox("faster"),
			new QCheckBox("fast"),
			new QCheckBox("medium"),
			new QCheckBox("slow"),
			new QCheckBox("slower"),
			new QCheckBox("veryslow"),
			new QCheckBox("placebo")
		};

		QGroupBox *pPresetGroupBox = new QGroupBox(tr("Preset selection"));
		QHBoxLayout* pPresetLayout = new QHBoxLayout();

		for (auto presetCheckBox: m_listPresetCheckbox) {
			presetCheckBox->setChecked(true);
			pPresetLayout->addWidget(presetCheckBox);
		}

		pPresetGroupBox->setLayout(pPresetLayout);

		pLayout->addWidget(pCRFGroupBox);
		pLayout->addWidget(pPresetGroupBox);

		pGroupBox->setLayout(pLayout);

		return pGroupBox;
	}

	QLayout* QBenchmarkConfigView::createControlButtons()
	{
		QHBoxLayout* pLayout = new QHBoxLayout();

		pLayout->addStretch();
		pLayout->addWidget(m_pStartButton);
		pLayout->addStretch();

		return pLayout;
	}
}

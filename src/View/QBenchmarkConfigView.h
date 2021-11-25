#ifndef Q_BENCHMARK_CONFIG_VIEW_H_
#define Q_BENCHMARK_CONFIG_VIEW_H_

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVector>
#include <QWidget>

namespace view {

	class QBenchmarkConfigView: public QWidget {
	Q_OBJECT

	public:
		QBenchmarkConfigView(QWidget* pParent);

	private:
		QGroupBox* createFileGroupBox();
		QGroupBox* createEncoderOptionsGroupBox();
		QLayout* createControlButtons();

	private:
		// Options for file selection
		QLabel* m_pFileLabel;
		QPushButton* m_pFileButton;

		// CRF options
		QSpinBox* m_pMinCRFSpinBox;
		QSpinBox* m_pMaxCRFSpinBox;

		// Present selection
		QVector<QCheckBox*> m_listPresetCheckbox;

		// Control button
		QPushButton* m_pStartButton;
	};

}

#endif // Q_BENCHMARK_CONFIG_VIEW_H_

#ifndef Q_BENCHMARK_CONFIG_VIEW_H_
#define Q_BENCHMARK_CONFIG_VIEW_H_

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVector>
#include <QWidget>

#include "Process/helper/CodecParameters.h"

namespace view {

	class QBenchmarkConfigView: public QWidget {
	Q_OBJECT

	public:
		QBenchmarkConfigView(QWidget* pParent);

		QString getSelectedFile() const;
		QCodecList getSelectedCodec() const;
		int getMinCRF() const;
		int getMaxCRF() const;
		QStringList getPresetList() const;

		void setSelectedFile(const QString& szFileName);

	private:
		QGroupBox* createFileGroupBox();
		QGroupBox* createCodecSelectionGroupBox();
		QGroupBox* createEncoderOptionsGroupBox();
		QLayout* createControlButtons();

	signals:
		void browseFileTriggered();
		void startBenchmarkTriggered();

	private:
		// Options for file selection
		QLineEdit* m_pFileLabel;
		QPushButton* m_pFileButton;

		// Option for codec selection
		QVector<QCheckBox*> m_listCodecCheckbox;

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

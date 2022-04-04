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

#include "Types/Codec.h"

namespace view {

	class QBenchmarkConfigView: public QWidget {
	Q_OBJECT

	public:
		QBenchmarkConfigView(QWidget* pParent);

		QString getSelectedFile() const;
		CodecList getSelectedCodec() const;
		int getMinCRF() const;
		int getMaxCRF() const;
		std::vector<std::string> getPresetList() const;

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

#include "Experiment.h"

#include "Process/helper/AVCodecHelper.h"
#include "Process/helper/VMAFWrapper.h"

#include "Configuration.h"

namespace {
	// TODO: Add support for other format
	VmafPixelFormat convertAVPixelFormat(AVPixelFormat pixelFormat)
	{
		switch (pixelFormat) {
		// case AV_PIX_FMT_GRAY8:
		// case AV_PIX_FMT_GRAY9:
		// case AV_PIX_FMT_GRAY10:
		// case AV_PIX_FMT_GRAY12:
		// case AV_PIX_FMT_GRAY14:
		// case AV_PIX_FMT_GRAY16:
		// 	return VMAF_PIX_FMT_YUV400P;

		case AV_PIX_FMT_YUV420P:
		case AV_PIX_FMT_YUV420P9:
		case AV_PIX_FMT_YUV420P10:
		case AV_PIX_FMT_YUV420P12:
		case AV_PIX_FMT_YUV420P14:
		case AV_PIX_FMT_YUV420P16:
			return VMAF_PIX_FMT_YUV420P;

		// case AV_PIX_FMT_YUV422P:
		// case AV_PIX_FMT_YUV422P9:
		// case AV_PIX_FMT_YUV422P10:
		// case AV_PIX_FMT_YUV422P12:
		// case AV_PIX_FMT_YUV422P14:
		// case AV_PIX_FMT_YUV422P16:
		// 	return VMAF_PIX_FMT_YUV422P;

		// case AV_PIX_FMT_YUV444P:
		// case AV_PIX_FMT_YUV444P9:
		// case AV_PIX_FMT_YUV444P10:
		// case AV_PIX_FMT_YUV444P12:
		// case AV_PIX_FMT_YUV444P14:
		// case AV_PIX_FMT_YUV444P16:
		// 	return VMAF_PIX_FMT_YUV444P;

		default:
			return VMAF_PIX_FMT_UNKNOWN;
		}

		return VMAF_PIX_FMT_UNKNOWN;
	}

	// TODO: Add support for other pixel depth
	int getPixelDepth(AVPixelFormat pixelFormat) {
		switch (pixelFormat) {
		case AV_PIX_FMT_GRAY8:
		case AV_PIX_FMT_YUV420P:
		case AV_PIX_FMT_YUV422P:
		case AV_PIX_FMT_YUV444P:
			return 8;

		// case AV_PIX_FMT_GRAY9:
		// case AV_PIX_FMT_YUV420P9:
		// case AV_PIX_FMT_YUV422P9:
		// case AV_PIX_FMT_YUV444P9:
		// 	return 9;

		// case AV_PIX_FMT_GRAY10:
		// case AV_PIX_FMT_YUV420P10:
		// case AV_PIX_FMT_YUV422P10:
		// case AV_PIX_FMT_YUV444P10:
		// 	return 10;

		// case AV_PIX_FMT_GRAY12:
		// case AV_PIX_FMT_YUV420P12:
		// case AV_PIX_FMT_YUV422P12:
		// case AV_PIX_FMT_YUV444P12:
		// 	return 12;

		// case AV_PIX_FMT_GRAY14:
		// case AV_PIX_FMT_YUV420P14:
		// case AV_PIX_FMT_YUV444P14:
		// case AV_PIX_FMT_YUV422P14:
		// 	return 14;

		// case AV_PIX_FMT_GRAY16:
		// case AV_PIX_FMT_YUV420P16:
		// case AV_PIX_FMT_YUV422P16:
		// case AV_PIX_FMT_YUV444P16:
		// 	return 16;

		default:
			return -1;
		}

		return -1;
	}
}

namespace vmaf {
	ExperimentThread::ExperimentThread(
		const QVector<QByteArray>& yuvFrames,
		const helper::avcodec::EncoderParameters& encoderParameters,
		QVector<Configuration>& listConfigurations,
		QMutex& mutexExperiments
	)
	: m_yuvFrames(yuvFrames)
	, m_encoderParameters(encoderParameters)
	, m_listConfiguration(listConfigurations)
	, m_mutexExperiments(mutexExperiments)
	{

	}

	ExperimentThread::ExperimentThread(ExperimentThread&& other)
	: m_yuvFrames(other.m_yuvFrames)
	, m_encoderParameters(other.m_encoderParameters)
	, m_listConfiguration(other.m_listConfiguration)
	, m_mutexExperiments(other.m_mutexExperiments)
	{

	}

	void ExperimentThread::run()
	{
		Configuration currentConfiguration;

		while (stoleTask(currentConfiguration)) {
			qDebug("TID: %p, CRF: %d, preset: %s", QThread::currentThreadId(), currentConfiguration.iCRF, qPrintable(currentConfiguration.szPreset));

			m_encoderParameters.iCRF = currentConfiguration.iCRF;
			m_encoderParameters.szPreset = currentConfiguration.szPreset;

			// Encode the video
			helper::avcodec::Context encoder;
			QVector<QByteArray> packets;

			if (encoder.encodeFrameStream(m_yuvFrames, m_encoderParameters, packets) != helper::avcodec::Error::Success) {
				qDebug("Encode error...");
				continue;
			}

			// Decode the transcoded video
			QVector<QByteArray> transcodedYUVFrames;
			helper::avcodec::Context decoder;
			if (decoder.decodePacketStream(packets, encoder.getCodec(), transcodedYUVFrames) != helper::avcodec::Error::Success) {
				qDebug("Decode transcoded video error...");
				continue;
			}

			if (m_yuvFrames.size() != transcodedYUVFrames.size()) {
				qDebug("Frame count mismatch...");
				continue;
			}

			// Call vmaf
			AVPixelFormat pixelFormat = m_encoderParameters.pixelFormat;
			helper::VMAFWrapper vmaf(convertAVPixelFormat(pixelFormat), getPixelDepth(pixelFormat), m_encoderParameters.iWidth, m_encoderParameters.iHeight);

			if (!vmaf.open()) {
				qDebug("VMAF error...");
				continue;
			}

			if (!vmaf.computeMetrics(m_yuvFrames, transcodedYUVFrames)) {
				qDebug("Compute VMAF error...");
				continue;
			}
		}
	}

	bool ExperimentThread::stoleTask(Configuration& configuration)
	{
		QMutexLocker locker(&m_mutexExperiments);

		// No last experiment
		if (m_listConfiguration.isEmpty()) {
			return false;
		}

		configuration = m_listConfiguration.takeLast();

		return true;
	}
}

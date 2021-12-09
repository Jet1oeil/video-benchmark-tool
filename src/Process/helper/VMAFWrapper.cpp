#include "VMAFWrapper.h"

#include <cassert>
#include <cstring>

#include <fstream>

namespace helper {
	VMAFWrapper::VMAFWrapper(VmafPixelFormat m_pixelFormat, int m_iPixelDepth, int m_iWidth, int m_iHeigh)
	: m_pContext(nullptr)
	, m_pModel(nullptr)
	, m_pixelFormat(m_pixelFormat)
	, m_iPixelDepth(m_iPixelDepth)
	, m_iWidth(m_iWidth)
	, m_iHeight(m_iHeigh)
	{

	}

	VMAFWrapper::~VMAFWrapper()
	{
		if (m_pModel != nullptr) {
			vmaf_model_destroy(m_pModel);
		}

		if (m_pContext != nullptr) {
			vmaf_close(m_pContext);
		}
	}

	bool VMAFWrapper::open()
	{
		VmafConfiguration cfg = {
			.log_level = VMAF_LOG_LEVEL_INFO,
			.n_threads = 1,
			.n_subsample = 0,
			.cpumask = 0,
		};

		if (vmaf_init(&m_pContext, cfg) != 0) {
			return false;
		}

		VmafModelConfig modelConfig;
		modelConfig.name = nullptr;
		modelConfig.flags = 0;

		if (vmaf_model_load(&m_pModel, &modelConfig, "vmaf_v0.6.1") != 0) {
			return false;
		}

		if (vmaf_use_features_from_model(m_pContext, m_pModel) != 0) {
			return false;
		}

		return true;
	}

	bool VMAFWrapper::computeMetrics(const QVector<QByteArray>& originalVideo, QVector<QByteArray> transcodedVideo)
	{
		// FIXME: We only support 420P 8 bits format
		assert(m_pixelFormat == VMAF_PIX_FMT_YUV420P);
		assert(m_iPixelDepth == 8);
		assert(originalVideo.size() == transcodedVideo.size());

		auto loadPicture = [&](const QByteArray& yuvFrame, VmafPicture* pPicture) {
			if (vmaf_picture_alloc(pPicture, m_pixelFormat, m_iPixelDepth, m_iWidth, m_iHeight) != 0) {
				return false;
			}

			const uint8_t* pCurrentSourceData = reinterpret_cast<const uint8_t*>(yuvFrame.data());
			for (int i = 0; i < 3; i++) {
				uint8_t* pCurrentDestionationData = static_cast<uint8_t*>(pPicture->data[i]);

				for (unsigned j = 0; j < pPicture->h[i]; j++) {
					std::memcpy(pCurrentDestionationData, pCurrentSourceData, sizeof(uint8_t) * pPicture->w[i]);
					pCurrentDestionationData += pPicture->stride[i];
					pCurrentSourceData += pPicture->w[i];
				}
			}

			return true;
		};

		for (int i = 0; i < originalVideo.size(); ++i) {
			qDebug("read frame #%d", i);

			const QByteArray& originalYUV = originalVideo[i];
			VmafPicture referencePicture;
			if (!loadPicture(originalYUV, &referencePicture)){
				qDebug("Error to load reference picture...");
				return false;
			}

			const QByteArray& transcodedYUV = transcodedVideo[i];
			VmafPicture transcodedPicture;
			if (!loadPicture(transcodedYUV, &transcodedPicture)){
				qDebug("Error to load reference picture...");
				return false;
			}

			if (vmaf_read_pictures(m_pContext, &referencePicture, &transcodedPicture, i) != 0) {
				qDebug("Error to read pictures...");
				return false;
			}
		}

		if (vmaf_read_pictures(m_pContext, nullptr, nullptr, 0) != 0) {
			qDebug("Error to flush context...");
			return false;
		}

		double dScore = 0.0;
		if (vmaf_score_pooled(m_pContext, m_pModel, VMAF_POOL_METHOD_MEAN, &dScore, 0, originalVideo.size() - 1) != 0) {
			qDebug("Error to get VMAF score...");
			return false;
		}

		qDebug("Score: %f", dScore);
		return true;
	}
}

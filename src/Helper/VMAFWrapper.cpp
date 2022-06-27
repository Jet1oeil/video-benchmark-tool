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

#include "VMAFWrapper.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <thread>

#include "Helper/Log.h"

namespace helper {
	VMAFWrapper::VMAFWrapper(VmafPixelFormat m_pixelFormat, int m_iPixelDepth, int m_iWidth, int m_iHeigh)
	: m_pContext(nullptr)
	, m_pModel(nullptr)
	, m_pixelFormat(m_pixelFormat)
	, m_iPixelDepth(m_iPixelDepth)
	, m_iWidth(m_iWidth)
	, m_iHeight(m_iHeigh)
	, m_dVMAFScore(0.0)
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
			.n_threads = std::thread::hardware_concurrency(),
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

	bool VMAFWrapper::computeMetrics(const types::FrameList& originalVideo, const types::FrameList& transcodedVideo)
	{
		// FIXME: We only support 420P 8 bits format
		assert(m_pixelFormat == VMAF_PIX_FMT_YUV420P);
		assert(m_iPixelDepth == 8);
		assert(originalVideo.size() == transcodedVideo.size());

		auto loadPicture = [&](const types::Frame& yuvFrame, VmafPicture* pPicture) {
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

		for (std::size_t i = 0; i < originalVideo.size(); ++i) {
			helper::Log::debug("read frame #%lu", i);

			const types::Frame& originalYUV = originalVideo[i];
			VmafPicture referencePicture;
			if (!loadPicture(originalYUV, &referencePicture)){
				helper::Log::error("Error to load reference picture...");
				return false;
			}

			const types::Frame& transcodedYUV = transcodedVideo[i];
			VmafPicture transcodedPicture;
			if (!loadPicture(transcodedYUV, &transcodedPicture)){
				helper::Log::error("Error to load reference picture...");
				return false;
			}

			if (vmaf_read_pictures(m_pContext, &referencePicture, &transcodedPicture, i) != 0) {
				helper::Log::error("Error to read pictures...");
				return false;
			}
		}

		if (vmaf_read_pictures(m_pContext, nullptr, nullptr, 0) != 0) {
			helper::Log::error("Error to flush context...");
			return false;
		}

		if (vmaf_score_pooled(m_pContext, m_pModel, VMAF_POOL_METHOD_MIN, &m_dVMAFScore, 0, originalVideo.size() - 1) != 0) {
			helper::Log::error("Error to get VMAF score...");
			return false;
		}

		return true;
	}

	double VMAFWrapper::getVMAFScore() const
	{
		return m_dVMAFScore;
	}
}

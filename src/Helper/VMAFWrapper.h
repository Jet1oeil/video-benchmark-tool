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

#ifndef HELPER_VMAF_WRAPPER_H_
#define HELPER_VMAF_WRAPPER_H_

#include <libvmaf/libvmaf.h>

#include "Types/Packet.h"

namespace helper {
	class VMAFWrapper {
	public:
		VMAFWrapper(VmafPixelFormat m_pixelFormat, int m_iPixelDepth, int m_iWidth, int m_iHeigh);
		~VMAFWrapper();

		VMAFWrapper(const VMAFWrapper&) = delete;
		VMAFWrapper(VMAFWrapper&&) = delete;

		VMAFWrapper& operator=(const VMAFWrapper&) = delete;
		VMAFWrapper& operator=(VMAFWrapper&&) = delete;

		bool open();
		bool computeMetrics(const types::PacketList& originalVideo, types::PacketList transcodedVideo);

		double getVMAFScore() const;

	private:
		VmafContext* m_pContext;
		VmafModel* m_pModel;
		VmafPixelFormat m_pixelFormat;
		int m_iPixelDepth;
		int m_iWidth;
		int m_iHeight;
		double m_dVMAFScore;
	};
}

#endif // HELPER_VMAF_WRAPPER_H_

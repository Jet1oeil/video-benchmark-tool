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

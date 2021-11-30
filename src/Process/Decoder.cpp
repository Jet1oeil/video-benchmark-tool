#include "Decoder.h"

#include <QString>

#include "helper/AVCodecHelper.h"
#include "helper/AVFormatHelper.h"

namespace proc {
	namespace Decoder {
		bool decodeVideoFile(const QString& szVideoFileName, QByteArray& yuvBytes)
		{
			avformat::Context formatContext;
			avcodec::Context codecContex;

			if (formatContext.openFile(qPrintable(szVideoFileName), codecContex) != avformat::Error::Success) {
				qDebug("Error format...");
				return false;
			}

			if (codecContex.open(formatContext) != avcodec::Error::Success) {
				qDebug("Error codec...");
				return false;
			}

			avcodec::Error errorCodec = avcodec::Error::Success;
			do {
				errorCodec = codecContex.decodePacket(formatContext, yuvBytes);
			} while (errorCodec == avcodec::Error::Success);

			if (errorCodec != avcodec::Error::Success && errorCodec != avcodec::Error::EndOfFile) {
				qDebug("Error decode...");
				return false;
			}

			return true;
		}
	}
}

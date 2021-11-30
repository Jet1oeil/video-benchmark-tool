#include "Decoder.h"

#include <QString>

#include "helper/AVCodecHelper.h"
#include "helper/AVFormatHelper.h"

namespace proc {
	namespace Decoder {
		bool decodeVideoFile(const QString& szVideoFileName, QByteArray& yuvBytes)
		{
			avcodec::Codec codec;
			avformat::Context formatContext;
			avformat::Stream videoStream;

			if (formatContext.openFile(qPrintable(szVideoFileName), videoStream, codec) != avformat::Error::Success) {
				qDebug("Error format...");
				return false;
			}

			avcodec::Context codecContex;
			if (codecContex.open(codec, videoStream) != avcodec::Error::Success) {
				qDebug("Error codec...");
				return false;
			}

			return true;
		}
	}
}

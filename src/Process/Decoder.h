#ifndef DECODER_H_
#define DECODER_H_

#include <QByteArray>

namespace proc {
	namespace Decoder {
		bool decodeVideoFile(const QString& szVideoFileName, QByteArray& yuvBytes);
	}
}

#endif // DECODER_H_

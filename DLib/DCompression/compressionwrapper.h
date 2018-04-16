#ifndef COMPRESSIONWRAPPER_H
#define COMPRESSIONWRAPPER_H

#include "../DTypes/types.h"

class DCompressor
{
public:
    typedef enum{
        NO_COMPRESSION = 0,
        BEST_SPEED = 1,
        BEST_COMPRESSION = 9,
        UBER_COMPRESSION = 10,
        DEFAULT_LEVEL = 6,
        DEFAULT_COMPRESSION = -1
    }COMPRESSION_LEVEL;

    static bool compressIt(QByteArray *pDest, const QByteArray& pSource, COMPRESSION_LEVEL level = DEFAULT_LEVEL);
    static bool uncompressIt(QByteArray *pDest, const QByteArray& pSource);

    static QByteArray zip(const QByteArray& pSource, COMPRESSION_LEVEL level = DEFAULT_LEVEL);
    static QByteArray unzip(const QByteArray& pSource);

private:
    DCompressor();
    virtual ~DCompressor();
};

#endif // COMPRESSIONWRAPPER_H

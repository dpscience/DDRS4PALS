#include "compressionwrapper.h"

#include "miniz.c"

bool DCompressor::compressIt(QByteArray *pDest, const QByteArray &pSource, DCompressor::COMPRESSION_LEVEL level)
{
    if ( !pDest )
        return false;


    unsigned char* toCompress = (unsigned char*)pSource.data();
    mz_ulong len = (mz_ulong)pSource.size();

    mz_ulong outputLen = len;
    unsigned char* compressedData = (unsigned char*)malloc((size_t)len);

    const int returnVal = mz_compress2(compressedData, &outputLen, toCompress, len, level);

    pDest->resize(outputLen);
    for ( int i = 0 ; i < outputLen ; ++ i )
        pDest->append(compressedData[i]);

    if ( returnVal == MZ_OK )
        return true;
    else
        return false;
}

bool DCompressor::uncompressIt(QByteArray *pDest, const QByteArray &pSource)
{
    if ( !pDest )
        return false;


    const unsigned char* toUncompress = (const unsigned char*)pSource.data();
    const mz_ulong len = (mz_ulong)pSource.size();

    mz_ulong outputLen = len;
    unsigned char* uncompressedData;

    const int returnVal = mz_uncompress(uncompressedData, &outputLen, toUncompress, len);

    pDest->resize(outputLen);
    for ( int i = 0 ; i < outputLen ; ++ i )
        pDest->append(uncompressedData[i]);

    if ( returnVal == MZ_OK )
        return true;
    else
        return false;
}

QByteArray DCompressor::unzip(const QByteArray &pSource)
{
    QByteArray pDest(0);

    const bool returnVal = uncompressIt(&pDest, pSource);
    DUNUSED_PARAM(returnVal);

    return pDest;
}

QByteArray DCompressor::zip(const QByteArray &pSource, DCompressor::COMPRESSION_LEVEL level)
{
    QByteArray pDest(0);

    const bool returnVal = compressIt(&pDest, pSource, level);
    DUNUSED_PARAM(returnVal);

    return pDest;
}

DCompressor::DCompressor() {}
DCompressor::~DCompressor() {}

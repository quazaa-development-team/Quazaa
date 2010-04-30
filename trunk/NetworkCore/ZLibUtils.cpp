#include "ZLibUtils.h"
#ifdef Q_OS_WIN32
#include "zlib/zlib.h"
#else
#include <zlib.h>
#endif

QMutex ZLibUtils::m_oMutex;
QByteArray ZLibUtils::m_oCompressBuffer;

bool ZLibUtils::Compress(QByteArray &pSrc, bool bIfSmaller)
{
    QMutexLocker l(&ZLibUtils::m_oMutex);

    if( bIfSmaller && pSrc.size() < 64 )
        return false;

    ulong nCompress = qMax(ZLibUtils::m_oCompressBuffer.size(), pSrc.size() * 2);

    ZLibUtils::m_oCompressBuffer.resize(nCompress);

    int nRet = ::compress((Bytef*)ZLibUtils::m_oCompressBuffer.data(), &nCompress, (const Bytef*)pSrc.data(), pSrc.size());

    if( nRet != Z_OK )
    {
        Q_ASSERT(nRet != Z_BUF_ERROR);
        return false;
    }

    ZLibUtils::m_oCompressBuffer.resize(nCompress);

    if( bIfSmaller && ZLibUtils::m_oCompressBuffer.size() > pSrc.size() )
        return false;

    pSrc.clear();
    pSrc.append(ZLibUtils::m_oCompressBuffer);

    return true;
}

bool ZLibUtils::Uncompress(QByteArray &pSrc)
{
    QMutexLocker l(&ZLibUtils::m_oMutex);

    for( ulong nSize = qMax(pSrc.size() * 6u, 1024u); ; nSize *= 2 )
    {
        ZLibUtils::m_oCompressBuffer.resize(nSize);

        ulong nCompress = ZLibUtils::m_oCompressBuffer.size();

        int nRet = ::uncompress((Bytef*)ZLibUtils::m_oCompressBuffer.data(), &nCompress, (Bytef*)pSrc.data(), pSrc.size());

        if( nRet == Z_OK )
        {
            ZLibUtils::m_oCompressBuffer.resize(nCompress);
            pSrc.clear();
            pSrc.append(ZLibUtils::m_oCompressBuffer);

            break;
        }

        if( nRet != Z_BUF_ERROR )
        {
            return false;
        }
    }

    return true;

}

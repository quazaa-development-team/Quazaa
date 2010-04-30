#ifndef ZLIBUTILS_H
#define ZLIBUTILS_H

#include <QByteArray>
#include <QMutex>
#include <QMutexLocker>

class ZLibUtils
{
public:

    static QMutex m_oMutex;
    static QByteArray m_oCompressBuffer;

    static bool Compress(QByteArray& pSrc, bool bIfSmaller = false);
    static bool Uncompress(QByteArray& pSrc);
};

#endif // ZLIBUTILS_H

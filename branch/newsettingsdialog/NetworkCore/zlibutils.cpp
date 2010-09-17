//
// zlibutils.cpp
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "zlibutils.h"
#include "zlib/zlib.h"

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

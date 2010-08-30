//
// sha1.h
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

#ifndef SHA1_H
#define SHA1_H

#include "NetworkCore/Hashes/AbstractHash.h"
#include <QCryptographicHash>

class CSHA1 : public CAbstractHash
{
    QCryptographicHash*  m_pHash;
public:
    CSHA1();
    CSHA1(CSHA1& rhs);
    ~CSHA1();

    inline static int ByteCount()
    {
        return 20;
    }

    void Finalize();
protected:
    void AddData_p(const char* pData, int nLength);
    void Clear_p();
    bool IsValid_p();
    QString ToURN_p();
    bool FromURN_p(const QString& sURN);
    QString ToString_p();
};

#endif // SHA1_H

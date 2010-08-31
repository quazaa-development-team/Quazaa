//
// AbstractHash.h
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

#ifndef ABSTRACTHASH_H
#define ABSTRACTHASH_H

#include "NetworkCore/types.h"

class CAbstractHash
{
protected:
    QByteArray m_baResult;
    bool       m_bFinalized;
public:
    CAbstractHash();
    CAbstractHash(CAbstractHash& rhs);

    void AddData(const char* pData, int nLength);
    void AddData(const QByteArray& baData);
    virtual void Finalize() = 0;

    bool FromRawData(const char* pData, int nLength);
    bool FromRawData(const QByteArray& baData);

    void Clear();

    bool IsValid();

    QString ToString();
    QByteArray RawResult();

    QString     ToURN();
    bool        FromURN(const QString& sURN);

protected:
    virtual void AddData_p(const char* pData, int nLength) = 0;
    virtual void Clear_p() = 0;
    virtual bool IsValid_p() = 0;
    virtual QString ToURN_p() = 0;
    virtual bool FromURN_p(const QString& sURN) = 0;
    virtual QString ToString_p() = 0;

};

#endif // ABSTRACTHASH_H

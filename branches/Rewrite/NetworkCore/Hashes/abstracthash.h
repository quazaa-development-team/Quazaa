/*
** abstracthash.h
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

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

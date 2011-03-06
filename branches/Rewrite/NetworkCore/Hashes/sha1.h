/*
** sha1.h
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

#ifndef SHA1_H
#define SHA1_H

#include "NetworkCore/Hashes/abstracthash.h"
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

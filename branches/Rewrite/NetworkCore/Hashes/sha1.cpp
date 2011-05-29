//
// sha1.cpp
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

#include "sha1.h"
#include "3rdparty/CyoEncode/CyoEncode.h"
#include "3rdparty/CyoEncode/CyoDecode.h"
#include <QByteArray>

CSHA1::CSHA1()
	: CAbstractHash()
{
	m_pHash = new QCryptographicHash(QCryptographicHash::Sha1);
	m_pHash->reset();
}

CSHA1::CSHA1(CSHA1& rhs)
{
	Q_UNUSED(rhs);

	m_pHash = m_pHash = new QCryptographicHash(QCryptographicHash::Sha1);
	m_pHash->reset();
}

CSHA1::~CSHA1()
{
	delete m_pHash;
}

void CSHA1::AddData_p(const char* pData, int nLength)
{
	m_pHash->addData(pData, nLength);
}

void CSHA1::Clear_p()
{
	m_pHash->reset();
}

bool CSHA1::IsValid_p()
{
	return m_baResult.size() == ByteCount();
}

void CSHA1::Finalize()
{
	m_baResult = m_pHash->result();
	m_bFinalized = true;
}

QString CSHA1::ToURN_p()
{
	return QString("urn:sha1:") + ToString();
}

bool CSHA1::FromURN_p(const QString& sURN)
{
	Q_UNUSED(sURN);

	return false;
}

QString CSHA1::ToString_p()
{
	QByteArray sRet;

	sRet.resize(cyoBase32EncodeGetLength(m_baResult.size()));
	cyoBase32Encode(sRet.data(), m_baResult.data(), m_baResult.size());

	return QString(sRet.data());
}

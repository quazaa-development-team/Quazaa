//
// abstracthash.cpp
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

#include "abstracthash.h"

CAbstractHash::CAbstractHash()
{
	m_bFinalized = false;
}
CAbstractHash::CAbstractHash(CAbstractHash &rhs)
{
	if( this != &rhs )
	{
		m_baResult = rhs.RawResult();
		m_bFinalized = true;
	}
}

void CAbstractHash::AddData(const char* pData, int nLength)
{
	Q_ASSERT(!m_bFinalized);
	m_bFinalized = false;
	AddData_p(pData, nLength);
}

void CAbstractHash::AddData(const QByteArray& baData)
{
	AddData(baData.constData(), baData.size());
}

bool CAbstractHash::FromRawData(const char* pData, int nLength)
{
	m_bFinalized = true;
	m_baResult.resize(nLength);
	memcpy(m_baResult.data(), pData, nLength);

	return IsValid();
}

bool CAbstractHash::FromRawData(const QByteArray& baData)
{
	return FromRawData(baData.constData(), baData.size());
}

void CAbstractHash::Clear()
{
	m_baResult.clear();
	m_bFinalized = false;
	Clear_p();
}

bool CAbstractHash::IsValid()
{
	if( !m_bFinalized )
		return false;

	return IsValid_p();
}

QString CAbstractHash::ToString()
{
	if( !m_bFinalized )
		Finalize();
	return ToString_p();
}

QByteArray CAbstractHash::RawResult()
{
	if( !m_bFinalized )
		Finalize();

	return m_baResult;
}

bool CAbstractHash::FromURN(const QString &sURN)
{
	Q_UNUSED(sURN);

	return false;
}

QString CAbstractHash::ToURN()
{
	if( !m_bFinalized )
		Finalize();

	return ToURN_p();
}

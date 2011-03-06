/*
** query.cpp
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

#include "query.h"
#include "g2packet.h"

CQuery::CQuery()
{
	m_nMinimumSize = 0;
	m_nMaximumSize = Q_UINT64_C(0xffffffffffffffff);
	m_pCachedPacket = 0;
}

void CQuery::SetGUID(QUuid& guid)
{
	m_oGUID = guid;
	if(m_pCachedPacket)
	{
		m_pCachedPacket->Release();
	}
}

void CQuery::SetDescriptiveName(QString sDN)
{
	m_sDescriptiveName = sDN;
	if(m_pCachedPacket)
	{
		m_pCachedPacket->Release();
	}
}
void CQuery::SetMetadata(QString sMeta)
{
	m_sMetadata = sMeta;
	if(m_pCachedPacket)
	{
		m_pCachedPacket->Release();
	}
}
void CQuery::SetSizeRestriction(quint64 nMin, quint64 nMax)
{
	m_nMinimumSize = nMin;
	m_nMaximumSize = nMax;
	if(m_pCachedPacket)
	{
		m_pCachedPacket->Release();
	}
}
void CQuery::AddURN(const char* pURN, quint32 nLength)
{
	m_lURNs.append(QString::fromAscii(pURN, nLength));
	if(m_pCachedPacket)
	{
		m_pCachedPacket->Release();
	}
}

G2Packet* CQuery::ToG2Packet(CEndPoint* pAddr, quint32 nKey)
{
	G2Packet* pPacket = G2Packet::New("Q2", true);

	//bool bWantURL = true;
	bool bWantDN = (!m_sDescriptiveName.isEmpty());
	bool bWantMD = !m_sMetadata.isEmpty();
	//bool bWantPFS = true;

	if(pAddr)
	{
		G2Packet* pUDP = pPacket->WritePacket("UDP", 10);
		pUDP->WriteHostAddress(pAddr);
		pUDP->WriteIntLE(nKey);
	}

	if(bWantDN)
	{
		pPacket->WritePacket("DN", m_sDescriptiveName.toUtf8().size())->WriteString(m_sDescriptiveName, false);
	}
	if(bWantMD)
	{
		pPacket->WritePacket("MD", m_sMetadata.toUtf8().size())->WriteString(m_sMetadata, false);
	}

	for(int i = 0; i < m_lURNs.size(); i++)
	{
		pPacket->WritePacket("URN", m_lURNs[i].size())->WriteString(m_lURNs[i], false);
	}

	/*if( m_nMinimumSize > 0 && m_nMaximumSize < 0xFFFFFFFFFFFFFFFF )
	{
		G2Packet* pSZR = pPacket->WriteChild("SZR");
		pSZR->WriteIntLE(m_nMinimumSize);
		pSZR->WriteIntLE(m_nMaximumSize);
	}
	else if( m_nMinimumSize > 0 )
	{
		G2Packet* pSZR = pPacket->WriteChild("SZR");
		pSZR->WriteIntLE(m_nMinimumSize);
		pSZR->WriteIntLE(0xFFFFFFFFFFFFFFFF);
	}
	else if( m_nMaximumSize < 0xFFFFFFFFFFFFFFFF )
	{
		G2Packet* pSZR = pPacket->WriteChild("SZR");
		pSZR->WriteIntLE(0);
		pSZR->WriteIntLE(m_nMaximumSize);
	}

	if( bWantURL || bWantDN || bWantMD || bWantPFS )
	{
		G2Packet* pInt = pPacket->WriteChild("I");
		if( bWantURL )
			pInt->WriteString("URL", true);
		if( bWantDN )
			pInt->WriteString("DN", true);
		if( bWantMD )
			pInt->WriteString("MD", true);
		if( bWantPFS )
			pInt->WriteString("PFS", true);
	}*/

	pPacket->WriteByte(0);
	pPacket->WriteGUID(m_oGUID);

	return pPacket;
}

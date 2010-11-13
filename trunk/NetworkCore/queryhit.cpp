//
// queryhit.cpp
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

#include "queryhit.h"
#include "g2packet.h"

CQueryHit::CQueryHit()
{
	m_pNext = 0;
	m_nObjectSize = 0;
	m_nCachedSources = 0;
	m_bIsPartial = 0;
	m_nPartialBytesAvailable = 0;
	m_bIsP2PIMCapable = false;
}
CQueryHit::~CQueryHit()
{
	qDebug() << "CQueryHit destructor";

	if(m_pNext)
	{
		m_pNext->Delete();
	}
}

QueryHitInfo* CQueryHit::ReadInfo(G2Packet* pPacket, IPv4_ENDPOINT* pSender)
{
	// do a shallow parsing...

	QueryHitInfo* pHitInfo = new QueryHitInfo();

	bool bHaveHits = false;
	bool bHaveNA = false;
	bool bHaveGUID = false;

	try
	{
		char szType[9];
		quint32 nLength = 0, nNext = 0;
		bool bCompound = false;

		if(pSender)
		{
			pHitInfo->m_oNodeAddress = *pSender;
			pHitInfo->m_oSenderAddress = *pSender;
			bHaveNA = true;
		}

		while(pPacket->ReadPacket(&szType[0], nLength, &bCompound))
		{
			nNext = pPacket->m_nPosition + nLength;

			if(strcmp("H", szType) == 0 && bCompound)
			{
				bHaveHits = true;
				continue;
			}

			if(bCompound)
			{
				pPacket->SkipCompound();
			}

			if(strcmp("NA", szType) == 0 && nLength >= 6)
			{
				IPv4_ENDPOINT oNodeAddr;
				pPacket->ReadHostAddress(&oNodeAddr);
				if(oNodeAddr.ip != 0 && oNodeAddr.port != 0)
				{
					pHitInfo->m_oNodeAddress = oNodeAddr;
					bHaveNA = true;
				}
			}
			else if(strcmp("GU", szType) == 0 && nLength >= 16)
			{
				QUuid oNodeGUID = pPacket->ReadGUID();
				if(!oNodeGUID.isNull())
				{
					pHitInfo->m_oNodeGUID = oNodeGUID;
					bHaveGUID = true;
				}
			}
			else if(strcmp("NH", szType) == 0 && nLength >= 6)
			{
				IPv4_ENDPOINT oNH;
				pPacket->ReadHostAddress(&oNH);
				if(oNH.ip != 0 && oNH.port != 0)
				{
					pHitInfo->m_lNeighbouringHubs.append(oNH);
				}
			}
			else if(strcmp("V", szType) == 0 && nLength >= 4)
			{
				pHitInfo->m_sVendor = pPacket->ReadString(4);
			}

			pPacket->m_nPosition = nNext;
		}
	}
	catch(...)
	{
		delete pHitInfo;
		return 0;
	}

	if(pPacket->GetRemaining() < 17 || !bHaveHits || !bHaveNA || !bHaveGUID)
	{
		qDebug() << "Malformatted hit in CSearchManager" << pPacket->GetRemaining() << bHaveHits << bHaveNA << bHaveGUID;
		delete pHitInfo;
		return 0;
	}

	pHitInfo->m_nHops = pPacket->ReadByte();
	pHitInfo->m_oGUID = pPacket->ReadGUID();

	return pHitInfo;
}

CQueryHit* CQueryHit::ReadPacket(G2Packet* pPacket, QueryHitInfo* pHitInfo)
{
	if(!pPacket->m_bCompound)
	{
		return 0;
	}

	pPacket->m_nPosition = 0; // reset packet position

	bool bHaveHits = false;
	bool bFirstHit = true;

	CQueryHit* pThisHit = new CQueryHit();

	try
	{
		char szType[9], szTypeX[9];
		quint32 nLength = 0, nLengthX = 0, nNext = 0, nNextX = 0;
		bool bCompound = false;

		while(pPacket->ReadPacket(&szType[0], nLength, &bCompound))
		{
			nNext = pPacket->m_nPosition + nLength;

			if(strcmp("H", szType) == 0 && bCompound)
			{
				CQueryHit* pHit = (bFirstHit ? pThisHit : new CQueryHit());

				if(!bFirstHit)
				{
					CQueryHit* pPrevHit = pThisHit;
					while(pPrevHit->m_pNext != 0)
					{
						pPrevHit = pPrevHit->m_pNext;
					}
					pPrevHit->m_pNext = pHit;
				}

				bool bHaveSize = false;
				QByteArray baTemp;
				bool bHaveDN = false;
				bool bHaveURN = false;

				while(pPacket->m_nPosition < nNext && pPacket->ReadPacket(&szTypeX[0], nLengthX))
				{
					nNextX = pPacket->m_nPosition + nLengthX;

					if(strcmp("URN", szTypeX) == 0)
					{
						QString sURN;
						char hashBuff[256];
						sURN = pPacket->ReadString();

						if(nLengthX >= 44u && sURN.compare("bp") == 0)
						{
							pPacket->Read(&hashBuff[0], CSHA1::ByteCount());
							if(pHit->m_oSha1.FromRawData(&hashBuff[0], CSHA1::ByteCount()))
							{
								bHaveURN = true;
							}
							else
							{
								pHit->m_oSha1.Clear();
							}
						}
						else if(nLengthX >= CSHA1::ByteCount() + 5u && sURN.compare("sha1") == 0)
						{
							pPacket->Read(&hashBuff[0], CSHA1::ByteCount());
							if(pHit->m_oSha1.FromRawData(&hashBuff[0], CSHA1::ByteCount()))
							{
								bHaveURN = true;
							}
							else
							{
								pHit->m_oSha1.Clear();
							}
						}


					}
					else if(strcmp("URL", szTypeX) == 0 && nLengthX)
					{
						// if url empty - try uri-res resolver or a node do not have this object
						// bez sensu...
						pHit->m_sURL = pPacket->ReadString();
					}
					else if(strcmp("DN", szTypeX) == 0)
					{
						if(bHaveSize)
						{
							pHit->m_sDescriptiveName = pPacket->ReadString(nLengthX);
						}
						else if(nLengthX > 4)
						{
							baTemp.resize(4);
							pPacket->Read(baTemp.data(), 4);
							pHit->m_sDescriptiveName = pPacket->ReadString(nLengthX - 4);
						}

						bHaveDN = true;
					}
					else if(strcmp("MD", szTypeX) == 0)
					{
						pHit->m_sMetadata = pPacket->ReadString();
					}
					else if(strcmp("SZ", szTypeX) == 0 && nLengthX >= 4)
					{
						if(nLengthX >= 8)
						{
							if(!baTemp.isEmpty())
							{
								pHit->m_sDescriptiveName.prepend(baTemp);
							}
							pHit->m_nObjectSize = pPacket->ReadIntLE<quint64>();
							bHaveSize = true;
						}
						else if(nLengthX >= 4)
						{
							if(!baTemp.isEmpty())
							{
								pHit->m_sDescriptiveName.prepend(baTemp);
							}
							pHit->m_nObjectSize = pPacket->ReadIntLE<quint32>();
							bHaveSize = true;
						}
					}
					else if(strcmp("CSC", szTypeX) == 0 && nLengthX >= 2)
					{
						pHit->m_nCachedSources = pPacket->ReadIntLE<quint16>();
					}
					else if(strcmp("PART", szTypeX) == 0 && nLengthX >= 4)
					{
						pHit->m_bIsPartial = true;
						pHit->m_nPartialBytesAvailable = pPacket->ReadIntLE<quint32>();
					}
					pPacket->m_nPosition = nNextX;
				}

				if(!bHaveSize && baTemp.size() == 4)
				{
					pHit->m_nObjectSize = qFromLittleEndian(*(quint32*)baTemp.constData());
				}
				else
				{
					pHit->m_sDescriptiveName.prepend(baTemp);
				}

				if(bHaveURN && bHaveDN)
				{
					bFirstHit = false;
					bHaveHits = true;
				}
				else
				{
					if(!bFirstHit)
					{
						// może teraz się nie wywali...
						// można by było to lepiej zrobić...
						for(CQueryHit* pTest = pThisHit; pTest != 0; pTest = pTest->m_pNext)
						{
							if(pTest->m_pNext == pHit)
							{
								pTest->m_pNext = 0;
								break;
							}
						}
						pHit->Delete();
					}
				}
			}

			pPacket->m_nPosition = nNext;
		}
	}
	catch(...) // packet incomplete, packet error, parser takes care of stream end
	{
		qDebug() << "EXCEPTION IN QUERY HIT PARSING!";
		if(pThisHit)
		{
			pThisHit->Delete();
		}
		//throw;
		return 0;
	}

	// we already know query hit informations, so don't reparse them
	if(!bHaveHits)
	{
		pThisHit->Delete();
		return 0;
	}

	CQueryHit* pHit = pThisHit;
	QSharedPointer<QueryHitInfo> pHitInfoS(pHitInfo);

	while(pHit != 0)
	{
		pHit->m_pHitInfo = pHitInfoS;

		pHit = pHit->m_pNext;
	}


	// TODO: sprawdzic poprawnosc hita... (Validate hit)

	pHit = pThisHit;

	while(pHit != 0)
	{
		pHit->ResolveURLs();
		pHit = pHit->m_pNext;
	}

	return pThisHit;
}

void CQueryHit::Delete()
{
	if(m_pNext)
	{
		m_pNext->Delete();
		m_pNext = 0;
	}

	delete this;
}
void CQueryHit::ResolveURLs()
{
	if(!m_sURL.isEmpty())
	{
		return;
	}

	/*if( m_lURNs.isEmpty() )
	    return;*/

	// TODO: odpowiednie kodowanie... (Appropriate Encoding)
	if(m_oSha1.IsValid())
	{
		m_sURL = m_sURL.sprintf("http://%s/uri-res/N2R?%s", m_pHitInfo->m_oNodeAddress.toString().toAscii().constData(), m_oSha1.ToURN().toAscii().constData());
	}
}
bool CQueryHit::IsValid(CQuery* pQuery)
{
	if(pQuery)
	{
		//
	}

	return true;
}

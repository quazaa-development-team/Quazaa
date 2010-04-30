#include "Query.h"
#include "g2packet.h"

CQuery::CQuery()
{
    m_nMinimumSize = 0;
    m_nMaximumSize = 0xFFFFFFFFFFFFFFFF;
    m_pCachedPacket = 0;
}

void CQuery::SetGUID(QUuid& guid)
{
    m_oGUID = guid;
    if( m_pCachedPacket )
        m_pCachedPacket->Release();
}

void CQuery::SetDescriptiveName(QString sDN)
{
    m_sDescriptiveName = sDN;
    if( m_pCachedPacket )
        m_pCachedPacket->Release();
}
void CQuery::SetMetadata(QString sMeta)
{
    m_sMetadata = sMeta;
    if( m_pCachedPacket )
        m_pCachedPacket->Release();
}
void CQuery::SetSizeRestriction(quint64 nMin, quint64 nMax)
{
    m_nMinimumSize = nMin;
    m_nMaximumSize = nMax;
    if( m_pCachedPacket )
        m_pCachedPacket->Release();
}
void CQuery::AddURN(const char *pURN, quint32 nLength)
{
    m_lURNs.append(QString::fromAscii(pURN, nLength));
    if( m_pCachedPacket )
        m_pCachedPacket->Release();
}

G2Packet* CQuery::ToG2Packet(IPv4_ENDPOINT *pAddr, quint32 nKey)
{
    G2Packet* pPacket = G2Packet::New("Q2");

    bool bWantURL = true;
    bool bWantDN = (!m_sDescriptiveName.isEmpty());
    bool bWantMD = !m_sMetadata.isEmpty();
    bool bWantPFS = true;

    if( pAddr )
    {
        G2Packet* pUDP = pPacket->WriteChild("UDP");
        pUDP->WriteHostAddress(pAddr);
        pUDP->WriteIntLE(nKey);
    }

    if( bWantDN )
    {
        pPacket->WriteChild("DN")->WriteString(m_sDescriptiveName, false);
    }
    if( bWantMD )
    {
        pPacket->WriteChild("MD")->WriteString(m_sMetadata, false);
    }

    for( int i = 0; i < m_lURNs.size(); i++ )
    {
        pPacket->WriteChild("URN")->WriteString(m_lURNs[i], false);
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

    pPacket->WriteGUID(m_oGUID);

    return pPacket;
}

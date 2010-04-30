#include "QueryHit.h"
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

    if( m_pNext )
        m_pNext->Delete();
}

CQueryHit* CQueryHit::ReadPacket(G2Packet *pPacket, IPv4_ENDPOINT *pAddress)
{
    if( !pPacket->HasChildren() )
        return 0;

    bool bHaveNA = false;
    bool bHaveGUID = false;
    bool bHaveHits = false;
    bool bFirstHit = true;

    CQueryHit* pThisHit = new CQueryHit();
    QSharedPointer<QueryHitInfo> pHitInfo = QSharedPointer<QueryHitInfo>(new QueryHitInfo());

    if( pAddress )
    {
        pHitInfo->m_oNodeAddress = *pAddress;
    }

    while( G2Packet* pChild = pPacket->ReadNextChild() )
    {
        if( pChild->IsType("NA") && pChild->PayloadLength() >= 6 )
        {
            IPv4_ENDPOINT oNodeAddr;
            pChild->ReadHostAddress(&oNodeAddr);
            if( oNodeAddr.ip != 0 && oNodeAddr.port != 0 )
            {
                pHitInfo->m_oNodeAddress = oNodeAddr;
                bHaveNA = true;
            }
        }
        else if( pChild->IsType("GU") && pChild->PayloadLength() >= 16 )
        {
            QUuid oNodeGUID = pChild->ReadGUID();
            if( !oNodeGUID.isNull() )
            {
                pHitInfo->m_oNodeGUID = oNodeGUID;
                bHaveGUID = true;
            }
        }
        else if( pChild->IsType("NH") && pChild->PayloadLength() >= 6 )
        {
            IPv4_ENDPOINT oNH;
            pChild->ReadHostAddress(&oNH);
            if( oNH.ip != 0 && oNH.port != 0 )
            {
                pHitInfo->m_lNeighbouringHubs.append(oNH);
            }
        }
        else if( pChild->IsType("H") && pChild->HasChildren() )
        {
            CQueryHit* pHit = (bFirstHit ? pThisHit : new CQueryHit());

            if( !bFirstHit )
            {
                CQueryHit* pPrevHit = pThisHit;
                while( pPrevHit->m_pNext != 0 )
                {
                    pPrevHit = pPrevHit->m_pNext;
                }
                pPrevHit->m_pNext = pHit;
            }

            bool bHaveSize = false;
            QByteArray baTemp;
            bool bHaveDN = false;
            bool bHaveURN = false;

            while( G2Packet* pH = pChild->ReadNextChild() )
            {
                if( pH->IsType("URN") )
                {
                    QString sURN;
                    char hashBuff[256];
                    sURN = pH->ReadString();

                    if( pH->PayloadLength() >= 44 && sURN.compare("bp") == 0 )
                    {
                        pH->ReadBytes(&hashBuff[0], CSHA1::ByteCount());
                        if( pHit->m_oSha1.FromRawData(&hashBuff[0], CSHA1::ByteCount()) )
                        {
                            bHaveURN = true;
                        }
                        else
                        {
                            pHit->m_oSha1.Clear();
                        }
                    }
                    else if( pH->PayloadLength() >= CSHA1::ByteCount() + 5 && sURN.compare("sha1") == 0 )
                    {
                        pH->ReadBytes(&hashBuff[0], CSHA1::ByteCount());
                        if( pHit->m_oSha1.FromRawData(&hashBuff[0], CSHA1::ByteCount()) )
                        {
                            bHaveURN = true;
                        }
                        else
                        {
                            pHit->m_oSha1.Clear();
                        }
                    }


                }
                else if( pH->IsType("URL") && pH->PayloadLength() )
                {
                    // if url empty - try uri-res resolver or a node do not have this object
                    // bez sensu...
                    pHit->m_sURL = pH->ReadString();
                }
                else if( pH->IsType("DN") )
                {
                    if( bHaveSize )
                    {
                        pHit->m_sDescriptiveName = pH->ReadString();
                    }
                    else if( pH->PayloadLength() > 4 )
                    {
                        baTemp.resize(4);
                        pH->ReadBytes(baTemp.data(), 4);
                        pHit->m_sDescriptiveName = pH->ReadString();
                    }

                    bHaveDN = true;
                }
                else if( pH->IsType("MD") )
                {
                    pHit->m_sMetadata = pH->ReadString();
                }
                else if( pH->IsType("SZ") && pH->PayloadLength() >= 4 )
                {
                    if( pH->PayloadLength() >= 8 )
                    {
                        if( !baTemp.isEmpty() )
                        {
                            pHit->m_sDescriptiveName.prepend(baTemp);
                        }
                        pHit->m_nObjectSize = pH->ReadIntLE<quint64>();
                        bHaveSize = true;
                    }
                    else if( pH->PayloadLength() >= 4 )
                    {
                        if( !baTemp.isEmpty() )
                        {
                            pHit->m_sDescriptiveName.prepend(baTemp);
                        }
                        pHit->m_nObjectSize = pH->ReadIntLE<quint32>();
                        bHaveSize = true;
                    }
                }
                else if( pH->IsType("CSC") && pH->PayloadLength() >= 2 )
                {
                    pHit->m_nCachedSources = pH->ReadIntLE<quint16>();
                }
                else if( pH->IsType("PART") && pH->PayloadLength() >= 4 )
                {
                    pHit->m_bIsPartial = true;
                    pHit->m_nPartialBytesAvailable = pH->ReadIntLE<quint32>();
                }

            }

            if( !bHaveSize && baTemp.size() == 4 )
            {
                pHit->m_nObjectSize = qFromLittleEndian(*(quint32*)baTemp.constData());
            }
            else
            {
                pHit->m_sDescriptiveName.prepend(baTemp);
            }

            if( bHaveURN && bHaveDN )
            {
                bFirstHit = false;
                bHaveHits = true;
            }
            else
            {
                if( !bFirstHit )
                {
                    // może teraz się nie wywali...
                    // można by było to lepiej zrobić...
                    for( CQueryHit* pTest = pThisHit; pTest != 0; pTest = pTest->m_pNext )
                    {
                        if( pTest->m_pNext == pHit )
                        {
                            pTest->m_pNext = 0;
                            break;
                        }
                    }
                    pHit->Delete();
                }
            }
        }
    }

    if( pPacket->PayloadLength() < 17 || !bHaveHits )
    {
        pThisHit->Delete();
        return 0;
    }

    quint8 nHops = pPacket->ReadIntLE<quint8>();
    QUuid oGUID = pPacket->ReadGUID();
    pHitInfo->m_oGUID = oGUID;
    pHitInfo->m_nHops = nHops;

    CQueryHit* pHit = pThisHit;

    while( pHit != 0 )
    {
        pHit->m_pHitInfo = pHitInfo;

        pHit = pHit->m_pNext;
    }


    // TODO: sprawdzic poprawnosc hita...

    pHit = pThisHit;

    while( pHit != 0 )
    {
        pHit->ResolveURLs();
        pHit = pHit->m_pNext;
    }

    return pThisHit;
}

void CQueryHit::Delete()
{
    if( m_pNext )
    {
        m_pNext->Delete();
        m_pNext = 0;
    }

    delete this;
}
void CQueryHit::ResolveURLs()
{
    if( !m_sURL.isEmpty() )
        return;

    /*if( m_lURNs.isEmpty() )
        return;*/

    // TODO: odpowiednie kodowanie...
    if( m_oSha1.IsValid() )
        m_sURL = m_sURL.sprintf("http://%s/uri-res/N2R?%s", m_pHitInfo->m_oNodeAddress.toString().toAscii().constData(), m_oSha1.ToURN().toAscii().constData());
}
bool CQueryHit::IsValid(CQuery *pQuery)
{
    if( pQuery )
    {
        //
    }

    return true;
}

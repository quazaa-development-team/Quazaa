#include "g2packet.h"

G2PacketPool G2Packets;

G2Packet::G2Packet()
{
    m_nReference = 0;
    Reset();
}


G2Packet* G2Packet::New(const char* pName)
{
    G2Packet* p = G2Packets.New();

    if( p )
    {
        memcpy(p->m_sName, pName, strlen(pName));
    }

    return p;
}

G2Packet* G2Packet::WriteChild(const char* pName)
{
    G2Packet* pChild = G2Packet::New(pName);

    if( pChild )
    {
        m_lChildren.append(pChild);
        m_bCompound = true;
    }

    return pChild;
}
void G2Packet::WriteBytes(const void* pData, quint32 nLength)
{
    m_sPayload.append((char*)pData, nLength);
}

G2Packet* G2Packet::FromBuffer(QByteArray& pBuffer)
{
    if( pBuffer.size() < 2 )
        throw packet_incomplete();

    char cb = pBuffer[0];

    if( cb == 0 )
    {
        pBuffer.remove(0, 1);
        throw stream_end();
    }

    quint32 nLenLen = ( cb & 0xc0 ) >> 6;
    quint32 nNameLen = ( cb & 0x38 ) >> 3;
    quint32 nFlags = ( cb & 0x07 );

    bool bCompound = ( nFlags & 0x04 ) ? true : false;
    bool bBigEndian = ( nFlags & 0x02 ) ? true : false;

    if( bBigEndian )
        throw packet_error();

    if( (quint32)pBuffer.size() < nLenLen + nNameLen + 2ul )
        throw packet_incomplete();

    quint32 nPacketLength = 0;
    memcpy((void*)&nPacketLength, pBuffer.constData() + 1, nLenLen);

    if( (quint32)pBuffer.size() < nLenLen + nNameLen + nPacketLength + 2ul )
        throw packet_incomplete();

	if( nPacketLength > 65536 )
		throw packet_error();

    pBuffer.remove(0, 1 + nLenLen); // cb + lenlen

    G2Packet* pPacket = G2Packet::New(pBuffer.left(nNameLen + 1));
    pBuffer.remove(0, nNameLen + 1); // name +1

    pPacket->m_sPayload = pBuffer.left(nPacketLength);
    pBuffer.remove(0, nPacketLength);

    pPacket->m_bCompound = bCompound;

    return pPacket;
}

G2Packet* G2Packet::ReadNextChild()
{
    if( !m_bCompound || m_bChildrenRead )
        return 0;

    try
    {
        G2Packet* pPacket = FromBuffer(m_sPayload);
        m_lChildren.append(pPacket);
        if( m_sPayload.size() == 0 )
            m_bChildrenRead = true;
        return pPacket;
    }
    catch( stream_end )
    {
        m_bChildrenRead = true;
        return 0;
    }
}

void G2Packet::SkipChildren()
{
    if( HasChildren() )
    {
        while( ReadNextChild() ) {}
    }
    m_bChildrenRead = true;

}

bool G2Packet::ReadBytes(void* pDest, quint32 nLength)
{
    if( m_sPayload.size() - m_nPosition < nLength )
        return false;

    if( !m_bChildrenRead )
        SkipChildren();

    memcpy(pDest, m_sPayload.constData() + m_nPosition, nLength);
    m_nPosition += nLength;
    //m_sPayload.remove(0, nLength);
    return true;
}
QString G2Packet::ReadString()
{
    qint32 nNullIndex = 0;

    char chZero = 0;

    nNullIndex = m_sPayload.indexOf(chZero, m_nPosition);
    if( nNullIndex == -1 )
        nNullIndex = m_sPayload.size();
    nNullIndex -= m_nPosition;

    char* pStart = m_sPayload.data() + m_nPosition;
    m_nPosition += nNullIndex + 1;
    m_nPosition = qMin<int>(m_nPosition, m_sPayload.size());

    return QString::fromUtf8(pStart, nNullIndex);
}
void G2Packet::WriteString(QString sToWrite, bool bTerminate)
{
    m_sPayload += sToWrite.toUtf8();
    if( bTerminate )
        m_sPayload += "\x00";
}

void G2Packet::Release()
{

	Q_ASSERT(m_nReference > 0);
    if( !m_nReference.deref() )
    {
        while( m_lChildren.size() )
            m_lChildren.takeFirst()->Release();

        G2Packets.Remove(this);
    }
}
void G2Packet::ToBuffer(QByteArray& pBuffer)
{
    qint32 nWriteOffset = pBuffer.size();
    bool bCompound = m_bCompound;

    if( m_lChildren.size() )
    {
        foreach(G2Packet* pChild, m_lChildren)
        {
            pChild->ToBuffer(pBuffer);
        }

        bCompound = true;
    }

    quint32 nLength = pBuffer.size() - nWriteOffset;

    if( bCompound && m_sPayload.size() )
    {
        pBuffer.append("\x00", 1);
        nLength += 1;
    }

    pBuffer.append(m_sPayload);
    nLength += m_sPayload.size();

    char c = 0;
    char lenlen = ((nLength & 0x000000ff)>0) + ((nLength & 0x0000ff00)>0) + ((nLength & 0x00ff0000)>0);
    char namelen = strlen(m_sName) - 1;

    c |= (lenlen << 6);
    c |= (namelen << 3);
    c |= (bCompound << 2);

    pBuffer.insert(nWriteOffset, c);
    pBuffer.insert(nWriteOffset + 1, (char*)&nLength, lenlen);
    pBuffer.insert(nWriteOffset + lenlen + 1, (char*)&m_sName, strlen(m_sName));

}



G2Packet* G2PacketPool::New()
{
    m_mLocker.lock();

    G2Packet* p = 0;

    if( m_lPacketsFree.size() )
    {
        p = m_lPacketsFree.takeFirst();
    }
    else
    {
        p = new G2Packet();
    }

    m_lPacketsUsed.insert(p);
    p->AddRef();

    m_mLocker.unlock();

    return p;
}
void G2PacketPool::Remove(G2Packet* pPacket)
{
    m_mLocker.lock();

    m_lPacketsFree.append(pPacket);
    m_lPacketsUsed.remove(pPacket);

    pPacket->Reset();

    m_mLocker.unlock();
}
G2PacketPool::~G2PacketPool()
{
    qDebug() << "G2PacketPool::~G2PacketPool() used: " << m_lPacketsUsed.size() << "free: " << m_lPacketsFree.size();
    while( m_lPacketsFree.size() )
        delete m_lPacketsFree.takeFirst();

    for( QSet<G2Packet*>::iterator i = m_lPacketsUsed.begin(); i != m_lPacketsUsed.end(); i++ )
    {
        qDebug() << "Used Packet: " << (*i)->m_sName << " ref count: " << (*i)->m_nReference;
    }
}

#include "sha1.h"
#include "3rdparty/CyoEncode/CyoEncode.h"
#include "3rdparty/CyoEncode/CyoDecode.h"
#include <QByteArray>

CSHA1::CSHA1()
    :CAbstractHash()
{
    m_pHash = new QCryptographicHash(QCryptographicHash::Sha1);
    m_pHash->reset();
}
CSHA1::CSHA1(CSHA1 &rhs)
{
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
    return false;
}

QString CSHA1::ToString_p()
{
    QByteArray sRet;

    sRet.resize(cyoBase32EncodeGetLength(m_baResult.size()));
    cyoBase32Encode(sRet.data(), m_baResult.data(), m_baResult.size());

    return QString(sRet.data());
}

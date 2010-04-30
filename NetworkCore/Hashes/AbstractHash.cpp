#include "AbstractHash.h"

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
    return false;
}
QString CAbstractHash::ToURN()
{
    if( !m_bFinalized )
        Finalize();

    return ToURN_p();
}

#include "SharedFile.h"
#include <QMetaType>

bool CSharedFile::m_bMetaRegistered = false;

CSharedFile::CSharedFile(QString &sFileName, QString &sPath, quint64 nFileSize, quint32 nTimestamp, bool bShared, CSHA1 &oSha1)
{
    m_sFilename = sFileName;
    m_nFileSize = nFileSize;
    m_nTimestamp = nTimestamp;
    m_bShared = bShared;
    m_sFullPath = sPath;
    m_oSha1 = oSha1;

    if( !CSharedFile::m_bMetaRegistered )
    {
        qRegisterMetaType<CSharedFilePtr>("CSharedFilePtr");
        CSharedFile::m_bMetaRegistered = true;
    }
}
CSharedFile::CSharedFile(const CSharedFile &rhs)
{
    m_sFilename = rhs.m_sFilename;
    m_nFileSize = rhs.m_nFileSize;
    m_nTimestamp = rhs.m_nTimestamp;
    m_bShared = rhs.m_bShared;
    m_sFullPath = rhs.m_sFullPath;
    m_oSha1 = rhs.m_oSha1;

    if( !CSharedFile::m_bMetaRegistered )
    {
        qRegisterMetaType<CSharedFilePtr>("CSharedFilePtr");
        CSharedFile::m_bMetaRegistered = true;
    }
}
CSharedFile::CSharedFile()
{
    m_bShared = false;
    m_nTimestamp = 0;
    m_nFileSize = 0;

    if( !CSharedFile::m_bMetaRegistered )
    {
        qRegisterMetaType<CSharedFilePtr>("CSharedFilePtr");
        CSharedFile::m_bMetaRegistered = true;
    }
}

bool operator==(CSharedFile lhs, CSharedFile rhs)
{
    return (lhs.m_sFilename == rhs.m_sFilename && lhs.m_sFullPath == rhs.m_sFullPath);
}

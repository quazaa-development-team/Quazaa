#include "SharedFiles.h"

#include <QFile>
#include <QDataStream>

CSharedFiles::CSharedFiles()
{
    Load();
}
CSharedFiles::~CSharedFiles()
{
    Save();
    m_lSharedFiles.clear();;
}

bool CSharedFiles::Seek(qint32 nRecord)
{
    m_nCurrentRecord = nRecord;
    if( m_nCurrentRecord < 0 )
        m_nCurrentRecord = -1;
    if( m_nCurrentRecord >= m_lSharedFiles.size() )
        m_nCurrentRecord = -1;

    return HasNext();
}
bool CSharedFiles::HasNext()
{
    // 0 < cur < size
    if( m_lSharedFiles.size() > 0 && m_nCurrentRecord + 1 >= 0 && m_nCurrentRecord < m_lSharedFiles.size() )
        return true;

    return false;
}
CSharedFilePtr CSharedFiles::Next()
{
    m_nCurrentRecord++;

    if( m_nCurrentRecord + 1 > m_lSharedFiles.size() )
    {
        Q_ASSERT(0);
    }

    return m_lSharedFiles[m_nCurrentRecord];
}

void CSharedFiles::Add(CSharedFilePtr pFile)
{
    m_lSharedFiles.append(pFile);
}

void CSharedFiles::Remove(CSharedFilePtr pFile)
{
    m_nCurrentRecord -= m_lSharedFiles.removeAll(pFile);
}

void CSharedFiles::Load()
{
    QFile f("share.dat");

    if( f.exists() && f.open(QFile::ReadOnly) )
    {
        QDataStream s(&f);

        quint32 nVersion = 0;
        s >> nVersion;

        if( nVersion == 1 )
        {
            while( !f.atEnd() )
            {
                CSharedFilePtr sf(new CSharedFile());
                QByteArray baHash;
                QByteArray baTemp;
                s >> sf->m_bShared;
                s >> sf->m_sFilename >> sf->m_nFileSize >> sf->m_nTimestamp >> baHash;
                s  >> sf->m_sFullPath;
                sf->m_oSha1.FromRawData(baHash);
                m_lSharedFiles.append(sf);
            }
        }

        f.close();

        qDebug() << "Loaded entries:" << m_lSharedFiles.size();
    }
}
void CSharedFiles::Save()
{
    QFile f("share.dat");

    if( f.open(QFile::WriteOnly) )
    {
        QDataStream s(&f);

        quint32 nVersion = 1;
        s << nVersion;

        for( int i = 0; i < m_lSharedFiles.size(); i++ )
        {
            CSharedFilePtr sf = m_lSharedFiles[i];
            s << sf->m_bShared << sf->m_sFilename << sf->m_nFileSize << sf->m_nTimestamp << sf->m_oSha1.RawResult() << sf->m_sFullPath;
        }

        f.close();
    }
}

CSharedFilePtr CSharedFiles::Find(QString sFullPath)
{
    for( int i = 0; i < m_lSharedFiles.size(); i++ )
    {
        CSharedFilePtr p = m_lSharedFiles[i];
        if( p->m_sFullPath == sFullPath )
            return p;
    }

    return CSharedFilePtr();
}

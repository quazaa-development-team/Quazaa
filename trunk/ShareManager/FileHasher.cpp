#include "FileHasher.h"
#include "Hashes/sha1.h"
#include <QFile>
#include <QByteArray>

QMutex CFileHasher::m_pSection;
QQueue<CSharedFilePtr> CFileHasher::m_lQueue;
CFileHasher** CFileHasher::m_pHashers = 0;
quint32  CFileHasher::m_nMaxHashers = 1;
quint32  CFileHasher::m_nRunningHashers = 0;

CFileHasher::CFileHasher(QObject* parent) :QThread(parent)
{
    m_bActive = true;
}
CFileHasher::~CFileHasher()
{
    m_bActive = false;
    if( isRunning() )
        wait();
}

CFileHasher* CFileHasher::HashFile(CSharedFilePtr pFile)
{
    m_pSection.lock();

    if( m_pHashers == 0 )
    {
        m_nMaxHashers = qMax<quint32>(2, QThread::idealThreadCount());
        m_pHashers = new CFileHasher*[m_nMaxHashers];
        for( int i = 0; i < m_nMaxHashers; i++ )
            m_pHashers[i] = 0;
    }

    //qDebug() << "File" << pFile->m_sFilename << "queued for hashing";
    m_lQueue.enqueue(pFile);

    CFileHasher* pHasher = 0;

    if( m_nRunningHashers < m_lQueue.size() && m_nRunningHashers < m_nMaxHashers )
    {
        for( int i = 0; i < m_nMaxHashers; i++ )
        {
            if( !m_pHashers[i] )
            {
                qDebug() << "Starting hasher: " << m_nRunningHashers;
                m_pHashers[i] = new CFileHasher();
                m_pHashers[i]->start(QThread::IdlePriority);
                m_nRunningHashers++;
                pHasher = m_pHashers[i];
                break;
            }
        }
    }

    m_pSection.unlock();

    return pHasher;
}
void CFileHasher::run()
{
    m_pSection.lock();

    while( !m_lQueue.isEmpty() )
    {
        CSharedFilePtr pFile = m_lQueue.dequeue();
        qDebug() << "Hashing" << pFile->m_sFilename;
        m_pSection.unlock();

        bool bHashed = true;

        QFile f;
        f.setFileName(pFile->m_sFullPath);

        if( f.exists() && f.open(QFile::ReadOnly) )
        {
            QByteArray baBuffer;
            baBuffer.resize(65536);

            pFile->m_oSha1.Clear();

            qint64 nTotalRead = 0;
            qint64 nLastPercent = -1;
            while( !f.atEnd() )
            {
                if( !m_bActive )
                {
                    qDebug() << "CFileHasher aborting...";
                    bHashed = false;
                    break;
                }
                qint64 nRead = f.read(baBuffer.data(), 65536);

                if( nRead < 0 )
                {
                    bHashed = false;
                    qDebug() << "File read error:" << f.error();
                    break;
                }
                else if ( nRead < 65536 )
                {
                    baBuffer.resize(nRead);
                }
                nTotalRead += nRead;

                pFile->m_oSha1.AddData(baBuffer);

                qint64 nPercent = (nTotalRead * 100 / f.size());
                if( nPercent != nLastPercent )
                {
                    nLastPercent = nPercent;
                    qDebug() << "Hashed " << nPercent << "%" << nTotalRead << " of " << f.size();
                }
            }

            f.close();
        }
        else
        {
            qDebug() << "File open error: " << f.error();
            bHashed = false;
        }

        if( bHashed )
        {
            pFile->m_oSha1.Finalize();
            qDebug() << "File " << pFile->m_sFilename << "hashed, sha1:" << pFile->m_oSha1.ToString();
            emit FileHashed(pFile);
        }

        m_pSection.lock();

        if( !m_bActive )
            break;

        if( bHashed && m_lQueue.isEmpty() )
        {
            // Zeby nie tworzyc co chwila nowych watkow, zaczekamy sobie kilka sekund, moze cos dojdzie do kolejki, jesli nie - konczymy
            m_pSection.unlock();
            sleep(10);
            m_pSection.lock();
        }
    }

    for( int i = 0; i < m_nMaxHashers; i++ )
    {
        if( m_pHashers[i] == this )
        {
            m_pHashers[i] = 0;
            deleteLater();
            m_nRunningHashers--;

            if( m_nRunningHashers == 0 )
            {
                delete [] m_pHashers;
                m_pHashers = 0;
            }
            break;
        }
    }

    m_pSection.unlock();

    qDebug() << "CFileHasher done. " << m_nRunningHashers;
}

#include "ShareManager.h"
#include <QTimer>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>

#include "ShareManager/FileHasher.h"

CThread ShareManagerThread;
CShareManager ShareManager;

CShareManager::CShareManager(QObject *parent) :
    QObject(parent), m_pWatcher()
{
    m_pSharedFiles = 0;
    m_nCurrentHasher = 0;
    m_nHashers = qMax<int>(2, QThread::idealThreadCount());
    m_pHashers = new CFileHasher*[m_nHashers];
    memset(m_pHashers, 0, sizeof(CFileHasher*) * m_nHashers);
}
CShareManager::~CShareManager()
{
    if( m_pSharedFiles )
    {
        m_pSharedFiles->Save();
        delete m_pSharedFiles;
    }
    if( m_pHashers )
        delete[] m_pHashers;

    if( ShareManagerThread.isRunning() )
        ShareManagerThread.exit(0);
}

void CShareManager::Initialize()
{
    m_pSection.lock();
    LoadDatabase();
    m_pSection.unlock();
    ShareManagerThread.start(&m_pSection, this);
}
void CShareManager::SetupThread()
{
    QTimer::singleShot(5000, this, SLOT(OnInitialize()));
}
void CShareManager::OnInitialize()
{
    m_pSection.lock();

    connect(&m_pWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(ScanFolder(QString)));

    qDebug() << "Scanning Shared folders...";

    ScanFolder("C:/Users/XXX/Downloads");
    m_pWatcher.addPath("C:/Users/XXX/Downloads");

    m_pSection.unlock();
}
void CShareManager::LoadDatabase()
{
    qDebug() << "Loading share database...";
    quint64 nFiles = 0;
    quint64 nTotalFiles = 0;

    if( !m_pSharedFiles )
    {

        m_pSharedFiles = new CSharedFiles();
        //m_pSharedFiles->Load();

        m_pSharedFiles->Seek();
        while( m_pSharedFiles->HasNext() )
        {
            CSharedFilePtr pNext = m_pSharedFiles->Next();

            nTotalFiles++;

            QFileInfo fi(pNext->m_sFullPath);

            if( !fi.exists() || fi.size() != pNext->m_nFileSize || fi.lastModified().toTime_t() != pNext->m_nTimestamp )
            {
                qDebug() << "Removing shared" << pNext->m_sFullPath;
                m_pSharedFiles->Remove(pNext);
                continue;
            }
            nFiles++;
        }
    }

    qDebug() << "Share database loaded, valid files found: " << nFiles << nTotalFiles;

}
void CShareManager::ScanFolder(QString sPath)
{
    QDir d(sPath);

    qDebug() << "Scanning " << sPath;

    if( d.exists() && d.isReadable() )
    {
        d.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        QFileInfoList list = d.entryInfoList();

        for( int i = 0; i < list.size(); i++ )
        {
            if( list.at(i).isDir() )
            {
                ScanFolder(list.at(i).absoluteFilePath());
            }
            else if( list.at(i).isFile() )
            {
                if( list.at(i).isReadable() && !m_pSharedFiles->Find(list.at(i).absoluteFilePath()) )
                {
                    CSharedFilePtr p(new CSharedFile);
                    p->m_sFilename = list.at(i).fileName();
                    p->m_sFullPath = list.at(i).absoluteFilePath();
                    p->m_nFileSize = list.at(i).size();
                    p->m_nTimestamp = list.at(i).lastModified().toTime_t();

                    /*if( m_pHashers[m_nCurrentHasher] == 0 )
                    {
                        m_pHashers[m_nCurrentHasher] = new CFileHasher();
                        connect(m_pHashers[m_nCurrentHasher], SIGNAL(FileHashed(CSharedFilePtr)), this, SLOT(OnFileHashed(CSharedFilePtr)));
                    }

                    m_pHashers[m_nCurrentHasher]->HashFile(p);
                    m_nCurrentHasher++;
                    if( m_nCurrentHasher > m_nHashers - 1 )
                        m_nCurrentHasher = 0;
                        */

                    CFileHasher* pHasher = CFileHasher::HashFile(p);
                    if( pHasher )
                    {
                        connect(pHasher, SIGNAL(FileHashed(CSharedFilePtr)), this, SLOT(OnFileHashed(CSharedFilePtr)), Qt::UniqueConnection);
                    }
                }
            }
        }
    }
    else
    {
        qDebug() << "Error opening " << sPath;
    }
}

void CShareManager::OnFileHashed(CSharedFilePtr pFile)
{
    m_pSection.lock();

    static quint32 nCounter = 0;
    nCounter++;
    if( nCounter % 100 == 0 )
    {
        qDebug() << "Saving shared files...";
        m_pSharedFiles->Save();
    }

    m_pSharedFiles->Add(pFile);

    m_pSection.unlock();
}

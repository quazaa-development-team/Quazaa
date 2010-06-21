#ifndef SHAREMANAGER_H
#define SHAREMANAGER_H

#include <QObject>
#include <QFileSystemWatcher>
#include "Thread.h"
#include "ShareManager/SharedFile.h"
#include "ShareManager/SharedFiles.h"

class CFileHasher;


class CShareManager : public QObject
{
    Q_OBJECT

public:
    QMutex m_pSection;

protected:
    CSharedFiles* m_pSharedFiles;
    CFileHasher** m_pHashers;
    quint32       m_nHashers;
    quint32       m_nCurrentHasher;

    QFileSystemWatcher  m_pWatcher;

public:
    CShareManager(QObject *parent = 0);
    ~CShareManager();
    void Initialize();
signals:

public slots:
    void SetupThread();
    void OnInitialize();
    void LoadDatabase();
    void ScanFolder(QString sPath);
    void OnFileHashed(CSharedFilePtr pFile);
};

extern CShareManager ShareManager;
extern CThread ShareManagerThread;
#endif // SHAREMANAGER_H

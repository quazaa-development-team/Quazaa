#ifndef FILEHASHER_H
#define FILEHASHER_H

#include <QThread>
#include <QMutex>
#include <QQueue>
#include "ShareManager/SharedFile.h"

class CFileHasher:public QThread
{
    Q_OBJECT
public:
    static QMutex   m_pSection;
    static QQueue<CSharedFilePtr> m_lQueue;
    static CFileHasher** m_pHashers;
    static quint32  m_nMaxHashers;
    static quint32  m_nRunningHashers;

    bool m_bActive;
public:
    CFileHasher(QObject* parent = 0);
    ~CFileHasher();
    static CFileHasher* HashFile(CSharedFilePtr pFile);
    void run();

signals:
    void FileHashed(CSharedFilePtr);
};

#endif // FILEHASHER_H

#ifndef SHAREDFILES_H
#define SHAREDFILES_H

#include "ShareManager/SharedFile.h"
#include <QList>

typedef QList<CSharedFilePtr> SharedFilesList;

class CSharedFiles
{
protected:
    SharedFilesList m_lSharedFiles;
    quint32         m_nCurrentRecord;

public:
    CSharedFiles();
    ~CSharedFiles();

    bool Seek(qint32 nRecord = -1);
    bool HasNext();
    CSharedFilePtr Next();

    void Add(CSharedFilePtr pFile);
    void Remove(CSharedFilePtr pFile);

    //SharedFilesList Find(QString& sName, quint64 nMinSize, quint64 nMaxSize);
    //CSharedFile&    Find(CSHA1& oSha1);
    CSharedFilePtr  Find(QString sFullPath);

    void Load();
    void Save();
};

#endif // SHAREDFILES_H


/*

  Tu ma byc zarzadzanie lista dzielonych plikow
  funkcje do dodawania, wywalania, szukania po hashu

  */

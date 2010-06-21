#ifndef SHAREDFILE_H
#define SHAREDFILE_H

#include "types.h"
#include <QSharedPointer>
#include "Hashes/sha1.h"

class CSharedFile
{
    static bool m_bMetaRegistered;
public:
    bool    m_bShared;

    // Atrybuty
    QString m_sFilename;
    quint64 m_nFileSize;
    quint32 m_nTimestamp;
    QString m_sFullPath;

    // Hashe
    CSHA1   m_oSha1;

    // Slowa kluczowe
    QStringList m_lKeywords;
public:
    CSharedFile(QString& sFileName, QString& sPath, quint64 nFileSize, quint32 nTimestamp, bool bShared, CSHA1& oSha1);
    CSharedFile(const CSharedFile& rhs);
    CSharedFile();
};

bool operator==(CSharedFile lhs, CSharedFile rhs);

typedef QSharedPointer<CSharedFile> CSharedFilePtr;
#endif // SHAREDFILE_H

/*
  Ma opisywac dzielony plik wraz z informacjami dla klasy hashujacej

  */


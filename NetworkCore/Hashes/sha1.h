#ifndef SHA1_H
#define SHA1_H

#include "NetworkCore/Hashes/AbstractHash.h"
#include <QCryptographicHash>

class CSHA1 : public CAbstractHash
{
    QCryptographicHash*  m_pHash;
public:
    CSHA1();
    CSHA1(CSHA1& rhs);
    ~CSHA1();

    inline static int ByteCount()
    {
        return 20;
    }

    void Finalize();
protected:
    void AddData_p(const char* pData, int nLength);
    void Clear_p();
    bool IsValid_p();
    QString ToURN_p();
    bool FromURN_p(const QString& sURN);
    QString ToString_p();
};

#endif // SHA1_H

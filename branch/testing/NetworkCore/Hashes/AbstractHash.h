#ifndef ABSTRACTHASH_H
#define ABSTRACTHASH_H

#include "NetworkCore/types.h"

class CAbstractHash
{
protected:
    QByteArray m_baResult;
    bool       m_bFinalized;
public:
    CAbstractHash();
    CAbstractHash(CAbstractHash& rhs);

    void AddData(const char* pData, int nLength);
    void AddData(const QByteArray& baData);
    virtual void Finalize() = 0;

    bool FromRawData(const char* pData, int nLength);
    bool FromRawData(const QByteArray& baData);

    void Clear();

    bool IsValid();

    QString ToString();
    QByteArray RawResult();

    QString     ToURN();
    bool        FromURN(const QString& sURN);

protected:
    virtual void AddData_p(const char* pData, int nLength) = 0;
    virtual void Clear_p() = 0;
    virtual bool IsValid_p() = 0;
    virtual QString ToURN_p() = 0;
    virtual bool FromURN_p(const QString& sURN) = 0;
    virtual QString ToString_p() = 0;

};

#endif // ABSTRACTHASH_H

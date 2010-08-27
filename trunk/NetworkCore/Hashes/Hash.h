#ifndef HASH_H
#define HASH_H

#include "types.h"

struct invalid_hash_exception{};

class CHash
{

public:
	enum Algorithm {SHA1, MD5, MD4};

protected:
	void*				m_pContext;
	bool				m_bFinalized;
	CHash::Algorithm	m_nHashAlgorithm;
	QByteArray			m_baRawValue;

public:
	CHash(const CHash& rhs);
	CHash(CHash::Algorithm algo);
	CHash(QByteArray baRaw, CHash::Algorithm algo);
	~CHash();

	static int	ByteCount(CHash::Algorithm algo);

	static CHash* FromURN(QString sURN);
	static CHash* FromRaw(QByteArray& baRaw, CHash::Algorithm algo);

	QString ToURN();
	QString ToString();
	QByteArray RawValue();

	void AddData(const char* pData, quint32 nLength);
	void AddData(QByteArray& baData);

	QString GetFamilyName();
	void Finalize()
	{
		RawValue();
	}
};

#endif // HASH_H

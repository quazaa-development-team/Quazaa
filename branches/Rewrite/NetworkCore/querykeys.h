#ifndef QUERYKEYS_H
#define QUERYKEYS_H

#include "types.h"

class QHostAddress;

class CQueryKeys
{
protected:
	quint32*	m_pTable;
	quint32		m_nTable;
public:
	CQueryKeys();
	~CQueryKeys();

	void Prepare();
	quint32 Create(QHostAddress pAddr);
	bool Check(QHostAddress pAddr, quint32 nKey);
};

extern CQueryKeys QueryKeys;

#endif // QUERYKEYS_H

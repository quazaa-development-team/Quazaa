#include "querykeys.h"
#include <limits>
#include <QHostAddress>

CQueryKeys QueryKeys;

CQueryKeys::CQueryKeys()
	: m_pTable(0), m_nTable(0)
{
}

CQueryKeys::~CQueryKeys()
{
	delete [] m_pTable;
}

void CQueryKeys::Prepare()
{
	m_nTable = 1u << 16;
	m_pTable = new quint32[m_nTable];

	for( uint i = 0; i < m_nTable; i++ )
	{
		m_pTable[i] = qrand() % std::numeric_limits<quint32>::max();
	}
}

quint32 CQueryKeys::Create(QHostAddress pAddr)
{
	if( !m_pTable )
		Prepare();

	int nHash = qHash(pAddr) % m_nTable;

	return m_pTable[nHash];
}
bool CQueryKeys::Check(QHostAddress pAddr, quint32 nKey)
{
	return (nKey == Create(pAddr));
}

#include "filefragments.h"
#include <QDebug>

using namespace boost::icl;

CFileFragments::CFileFragments(quint64 nFileLength) :
	m_nFileLength(nFileLength)
{

}

bool CFileFragments::insert(const quint64 nBegin, const quint64 nEnd)
{
	if( nBegin < 0 || nEnd > m_nFileLength )
		return false;

	m_lFragments.insert(discrete_interval<quint64>(nBegin, nEnd));

	return true;
}

QPair<quint64, quint64> CFileFragments::largestEmpty()
{
	interval_set<quint64> emptyParts;
	emptyParts.insert(discrete_interval<quint64>(0, m_nFileLength));

	emptyParts -= m_lFragments;

	QPair<quint64, quint64> ret = qMakePair<quint64, quint64>(0, 0);
	quint64 nLargest = 0;

	for(interval_set<quint64>::iterator itCurr = emptyParts.begin(); itCurr != emptyParts.end(); ++itCurr)
	{
		quint64 nSize = (*itCurr).upper() - (*itCurr).lower();

		if( nSize > nLargest )
		{
			ret.first = (*itCurr).lower();
			ret.second = (*itCurr).upper();
			nLargest = nSize;
		}
	}

	return ret;
}

QString CFileFragments::available()
{
	QString sRet;

	for(interval_set<quint64>::iterator it = m_lFragments.begin(); it != m_lFragments.end(); ++it)
	{
		sRet += QString("%1-%2,").arg((*it).lower()).arg((*it).upper());
	}

	if( !m_lFragments.empty() )
		sRet.chop(1);

	return sRet;
}

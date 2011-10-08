#include "filefragments.h"

using namespace boost::icl;

CFileFragments::CFileFragments(quint64 nFileLength) :
	m_nFileLength(nFileLength)
{

}

bool CFileFragments::insert(const quint64 nBegin, const quint64 nEnd)
{
	if( nBegin < 0 || nEnd > m_nFileLength )
		return false;

	m_lFragments.insert(discrete_interval<quint64>(nBegin, nEnd, boost::icl::interval_bounds::closed()));

	return true;
}

QPair<quint64, quint64> CFileFragments::largestEmpty(const CFileFragments &oExcept)
{
	interval_set<quint64> emptyParts;
	// Intervals in m_lFragments are byte offsets [start,end]
	// left open interval means add 1 to lower bound
	// right open interval means subtract 1 from upper bound
	// we start with full file size (m_nFileLength - 1 = last byte) as right open interval
	emptyParts.insert(discrete_interval<quint64>(0, m_nFileLength, boost::icl::interval_bounds::right_open()));

	// subtract "full" fragments
	emptyParts -= m_lFragments;
	emptyParts -= oExcept.m_lFragments;

	QPair<quint64, quint64> ret = qMakePair<quint64, quint64>(0, 0);
	quint64 nLargest = 0;

	for(interval_set<quint64>::iterator itCurr = emptyParts.begin(); itCurr != emptyParts.end(); ++itCurr)
	{
		quint64 nSize = (*itCurr).upper() - (*itCurr).lower();

		if( nSize > nLargest )
		{
			boost::icl::interval_bounds bounds = (*itCurr).bounds();

			ret.first = (*itCurr).lower();
			if( bounds == boost::icl::interval_bounds::open() || bounds == boost::icl::interval_bounds::left_open() )
				ret.first++;

			ret.second = (*itCurr).upper();
			if( bounds == boost::icl::interval_bounds::open() || bounds == boost::icl::interval_bounds::right_open() )
				ret.second--;

			nLargest = nSize;
		}
	}

	return ret;
}

QPair<quint64, quint64> CFileFragments::smallestEmpty(const CFileFragments &oExcept)
{
	interval_set<quint64> emptyParts;
	// Intervals in m_lFragments are byte offsets [start,end]
	// left open interval means add 1 to lower bound
	// right open interval means subtract 1 from upper bound
	// we start with full file size (m_nFileLength - 1 = last byte) as right open interval
	emptyParts.insert(discrete_interval<quint64>(0, m_nFileLength, boost::icl::interval_bounds::right_open()));

	// subtract "full" fragments
	emptyParts -= m_lFragments;
	emptyParts -= oExcept.m_lFragments;

	QPair<quint64, quint64> ret = qMakePair<quint64, quint64>(0, 0);
	quint64 nSmallest = 0;

	for(interval_set<quint64>::iterator itCurr = emptyParts.begin(); itCurr != emptyParts.end(); ++itCurr)
	{
		quint64 nSize = (*itCurr).upper() - (*itCurr).lower();

		if( nSize < nSmallest )
		{
			boost::icl::interval_bounds bounds = (*itCurr).bounds();

			ret.first = (*itCurr).lower();
			if( bounds == boost::icl::interval_bounds::open() || bounds == boost::icl::interval_bounds::left_open() )
				ret.first++;

			ret.second = (*itCurr).upper();
			if( bounds == boost::icl::interval_bounds::open() || bounds == boost::icl::interval_bounds::right_open() )
				ret.second--;

			nSmallest = nSize;
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


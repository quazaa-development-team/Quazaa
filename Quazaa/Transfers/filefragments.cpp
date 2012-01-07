/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2012.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public 
** License version 3.0 requirements will be met: 
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version 
** 3.0 along with Quazaa; if not, write to the Free Software Foundation, 
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "filefragments.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

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


#ifndef FILEFRAGMENTS_H
#define FILEFRAGMENTS_H

#include "types.h"
#include <3rdparty/boost/icl/interval_set.hpp>
#include <3rdparty/boost/icl/discrete_interval.hpp>

class CFileFragments
{
protected:
	boost::icl::interval_set<quint64> m_lFragments;
	quint64	m_nFileLength;
public:
	CFileFragments(quint64 nFileLength = 0);
	bool insert(const quint64 nBegin, const quint64 nEnd);
	QPair<quint64, quint64> largestEmpty(const CFileFragments& oExcept = CFileFragments());
	QPair<quint64, quint64> smallestEmpty(const CFileFragments& oExcept = CFileFragments());
	QString available();
public:
	inline quint64 size();
	inline void erase(quint64 nBegin, quint64 nEnd);
	inline void setSize(quint64 nSize);
};

quint64 CFileFragments::size()
{
	return m_lFragments.size();
}
void CFileFragments::erase(quint64 nBegin, quint64 nEnd)
{
	m_lFragments.erase(boost::icl::discrete_interval<quint64>(nBegin, nEnd));
}
void CFileFragments::setSize(quint64 nSize)
{
	m_nFileLength = nSize;
}

#endif // FILEFRAGMENTS_H

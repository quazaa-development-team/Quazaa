/*
** $Id: Range.hpp 869 2012-01-16 18:49:34Z brov $
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

// FileFragments code borrowed from Shareaza
// http://shareaza.sourceforge.net/

#ifndef FILEFRAGMENTS_RANGE_HPP_INCLUDED
#define FILEFRAGMENTS_RANGE_HPP_INCLUDED

namespace Ranges
{

// helper class that stands for ... nothing
class EmptyType
{
public:
	// Anything can be turned into nothing
	template< typename T >
	EmptyType(const T&) { }
	// From nothing we get something ;)
	EmptyType() { }
};

template< typename SizeT, class PayloadT = EmptyType >
class Range : private PayloadT
{
public:
	typedef SizeT size_type;
	typedef PayloadT payload_type;
	typedef RangeError< Range > RangeException;
	Range(size_type begin, size_type end, const payload_type& payload = payload_type())
	: PayloadT( payload ), m_range( begin, end )
	{
		if ( end < begin )
		{
			m_range.first = ~0ULL - 1000;
			m_range.second = ~0ULL;

			qDebug( qPrintable(QString("RangeError - default - begin: %1 - end: %2").arg(begin).arg(end)) );
		}
	}   
	Range(std::pair< size_type, size_type > range, const payload_type& payload = payload_type())
	: PayloadT( payload ), m_range( range )
	{
		if ( range.second < range.first )
		{
			m_range.first = ~0ULL - 1000;
			m_range.second = ~0ULL;

			qDebug( qPrintable(QString("RangeError - pair - begin: %1 - end: %2").arg(range.first).arg(range.second)) );
		}
	}

	// conversion - will succeed if payload is convertible
	template< class OtherPayloadT >
	Range(const Range< size_type, OtherPayloadT >& other)
	: PayloadT( other.value() ), m_range( other.m_range )
	{ }

	size_type begin() const { return m_range.first; }
	size_type end() const { return m_range.second; }
	size_type size() const { return end() - begin(); }

	payload_type& value() { return *this; }
	const payload_type& value() const { return *this; }
private:
	std::pair< size_type, size_type > m_range;
};

template< typename SizeT, class PayloadT >
bool operator==(const Range< SizeT, PayloadT >& lhs, const Range< SizeT, PayloadT >& rhs)
{
	return lhs.begin() == rhs.begin() && lhs.end() == rhs.end() && lhs.value() == rhs.value();
}
template< typename SizeT >
bool operator==(const Range< SizeT >& lhs, const Range< SizeT >& rhs)
{
	return lhs.begin() == rhs.begin() && lhs.end() == rhs.end();
}

template< typename SizeT, class PayloadT >
bool operator!=(const Range< SizeT, PayloadT >& lhs, const Range< SizeT, PayloadT >& rhs)
{
	return !( lhs == rhs );
}

template< typename SizeT, class PayloadT >
struct RangeCompare
: public std::binary_function< Range< SizeT, PayloadT >, Range< SizeT, PayloadT >, bool >
{
	typename RangeCompare<SizeT, PayloadT>::result_type operator()(typename RangeCompare<SizeT, PayloadT>::first_argument_type lhs, typename RangeCompare<SizeT, PayloadT>::second_argument_type rhs) const
	{
		return lhs.end() <= rhs.begin();
	}
};

} // namespace Ranges

#endif // #ifndef FILEFRAGMENTS_RANGE_HPP_INCLUDED

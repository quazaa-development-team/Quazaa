/*
** hostcachehost.cpp
**
** Copyright © Quazaa Development Team, 2009-2013.
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

#include "network.h"
#include "hostcachehost.h"
#include "quazaasettings.h"

template<>
class qLess <CHostCacheHost*>
{
public:
	inline bool operator()(const CHostCacheHost* l, const CHostCacheHost* r) const
	{
		return l->m_tTimestamp > r->m_tTimestamp;
	}
};

bool CHostCacheHost::canQuery(QDateTime tNow)
{
	if ( tNow.isNull() )
	{
		tNow = common::getDateTimeUTC();
	}

	if ( !m_tAck.isNull() && m_nQueryKey ) // if waiting for an ack, and we have a query key
	{
		return false;
	}

	if ( m_tTimestamp.secsTo( tNow ) > quazaaSettings.Gnutella2.HostCurrent ) // host is not too old
	{
		return false;
	}

	if ( !m_tRetryAfter.isNull() && tNow < m_tRetryAfter ) // honor retry-after
	{
		return false;
	}

	if ( m_tLastQuery.isNull() ) // host not already queried
	{
		return true;
	}

	return m_tLastQuery.secsTo(tNow) > (long)quazaaSettings.Gnutella2.QueryHostThrottle;
}

void CHostCacheHost::setKey(quint32 nKey, CEndPoint* pHost)
{
	m_tAck      = QDateTime();
	m_nFailures = 0;
	m_nQueryKey = nKey;
	m_nKeyTime  = common::getDateTimeUTC();
	m_nKeyHost  = pHost ? *pHost : Network.GetLocalAddress();
}

/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2013.
** This file is part of the Quazaa Security Library (quazaa.sourceforge.net)
**
** The Quazaa Security Library is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** The Quazaa Security Library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with the Quazaa Security Library; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "securitymanager.h"
#include "securerule.h"

#include "debug_new.h"

/**
 * @brief CSecureRule::CSecureRule Constructor.
 * @param bCreate
 */
CSecureRule::CSecureRule()
{
	// This invalidates rule as long as it does not contain any useful content.
	m_nType   = srContentUndefined;

	m_nAction = srDeny;
	m_tExpire = srIndefinite;
	m_nToday  = 0;
	m_nTotal  = 0;

	m_oUUID   = QUuid::createUuid();
}

CSecureRule::CSecureRule(const CSecureRule& pRule)
{
	// The usage of a custom copy constructor makes sure the list of registered
	// pointers is NOT forwarded to a copy of this rule.

	m_nType		= pRule.m_nType;
	m_sContent	= pRule.m_sContent;
	m_nToday	= pRule.m_nToday;
	m_nTotal	= pRule.m_nTotal;
	m_nAction	= pRule.m_nAction;
	m_oUUID		= pRule.m_oUUID;
	m_tExpire	= pRule.m_tExpire;
	m_sComment	= pRule.m_sComment;
}

CSecureRule::~CSecureRule()
{
	// Set all pointers to this rule to NULL to notify them about the deletion of this object.
	for ( std::list<CSecureRule**>::iterator i = m_lPointers.begin();
		  i != m_lPointers.end(); ++i )
	{
		*(*i) = NULL;
	}
}

CSecureRule* CSecureRule::getCopy() const
{
	// This method should never be called.
	Q_ASSERT( false );

	return new CSecureRule( *this );
}

bool CSecureRule::operator==(const CSecureRule& pRule) const
{
	return ( m_nType	== pRule.m_nType	&&
			 m_nAction	== pRule.m_nAction	&&
			 m_sComment	== pRule.m_sComment	&&
			 m_oUUID	== pRule.m_oUUID	&&
			 m_tExpire	== pRule.m_tExpire	&&
			 m_sContent	== pRule.m_sContent);
}

bool CSecureRule::operator!=(const CSecureRule& pRule) const
{
	return !( *this == pRule );
}

bool CSecureRule::parseContent(const QString&)
{
	Q_ASSERT( false );
	return false;
}

QString CSecureRule::getContentString() const
{
	Q_ASSERT( m_nType != srContentUndefined );

	return m_sContent;
}

void CSecureRule::registerPointer(CSecureRule** pRule)
{
	m_lPointers.push_back( pRule );
}

void CSecureRule::unRegisterPointer(CSecureRule** pRule)
{
	m_lPointers.remove( pRule );
}

//////////////////////////////////////////////////////////////////////
// CSecureRule match

bool CSecureRule::match(const CEndPoint&) const
{
	return false;
}
bool CSecureRule::match(const QString&) const
{
	return false;
}
bool CSecureRule::match(const CQueryHit* const) const
{
	return false;
}
bool CSecureRule::match(const QList<QString>&, const QString&) const
{
	return false;
}

//////////////////////////////////////////////////////////////////////
// CSecureRule serialize

void CSecureRule::save(const CSecureRule* const pRule, QDataStream &oStream)
{
	oStream << (quint8)(pRule->m_nType);
	oStream << (quint8)(pRule->m_nAction);
	oStream << pRule->m_sComment;
	oStream << pRule->m_oUUID.toString();
	oStream << pRule->m_tExpire;
	oStream << pRule->m_nTotal.loadAcquire();

	oStream << pRule->getContentString();

	if ( pRule->m_nType == srContentUserAgent )
	{
		oStream << ((CUserAgentRule*)pRule)->getRegExp();
	}
	else if ( pRule->m_nType == srContentText )
	{
		oStream << ((CContentRule*)pRule)->getAll();
	}
}

void CSecureRule::load(CSecureRule*& pRule, QDataStream &fsFile, int)
{
	if ( pRule )
	{
		delete pRule;
		pRule = NULL;
	}

	quint8		nType;
	quint8		nAction;
	QString		sComment;
	QString		sUUID;
	quint32		tExpire;
	quint32		nTotal;

	fsFile >> nType;
	fsFile >> nAction;
	fsFile >> sComment;
	fsFile >> sUUID;
	fsFile >> tExpire;
	fsFile >> nTotal;

	QString sTmp;
	bool	bTmp = true;

	switch ( nType )
	{
	case 0:
		// contentless rule
		pRule = new CSecureRule();
		Q_ASSERT( false );
		break;
	case 1:
		pRule = new CIPRule();
		fsFile >> sTmp;
		pRule->parseContent( sTmp );
		break;
	case 2:
		pRule = new CIPRangeRule();
		fsFile >> sTmp;
		pRule->parseContent( sTmp );
		break;
	case 3:
#if SECURITY_ENABLE_GEOIP
		pRule = new CCountryRule();
		fsFile >> sTmp;
		pRule->parseContent( sTmp );
#endif
		break;
	case 4:
		pRule = new CHashRule();
		fsFile >> sTmp;
		pRule->parseContent( sTmp );
		break;
	case 5:
		pRule = new CRegExpRule();
		fsFile >> sTmp;
		pRule->parseContent( sTmp );
		break;
	case 6:
		pRule = new CUserAgentRule();
		fsFile >> sTmp;
		fsFile >> bTmp;
		pRule->parseContent( sTmp );
		((CUserAgentRule*)pRule)->setRegExp( bTmp );
		break;
	case 7:
		pRule = new CContentRule();
		fsFile >> sTmp;
		fsFile >> bTmp;
		pRule->parseContent( sTmp );
		((CContentRule*)pRule)->setAll( bTmp );
		break;
	default:
		Q_ASSERT( false );
		break;
	}

	if ( pRule )
	{
		pRule->m_nAction  = (TPolicy)nAction;
		pRule->m_sComment = sComment;
		pRule->m_oUUID    = QUuid( sUUID );
		pRule->m_tExpire  = tExpire;
		pRule->m_nTotal.storeRelease(nTotal);
	}
}

//////////////////////////////////////////////////////////////////////
// CSecureRule XML

CSecureRule* CSecureRule::fromXML(QXmlStreamReader& oXMLdocument, float nVersion)
{
	QXmlStreamAttributes attributes = oXMLdocument.attributes();

	const QString sType = attributes.value( "type" ).toString();

	if ( sType.isEmpty() )
		return NULL;

	CSecureRule* pRule = NULL;

	if ( sType.compare( "address", Qt::CaseInsensitive ) == 0 )
	{
		QString sAddress = attributes.value( "address" ).toString();

		pRule = new CIPRule();
		pRule->parseContent( sAddress );
	}
	else if ( sType.compare( "addressrange", Qt::CaseInsensitive ) == 0 )
	{
		QString sStartAddress = attributes.value( "startaddress" ).toString();
		QString sEndAddress = attributes.value( "endaddress" ).toString();

		pRule = new CIPRangeRule();
		pRule->parseContent( QString("%1-%2").arg(sStartAddress).arg(sEndAddress) );
	}
	else if ( sType.compare( "hash", Qt::CaseInsensitive ) == 0 )
	{
		CHashRule* rule = new CHashRule();
		if ( !rule->parseContent( attributes.value( "content" ).toString() ) )
		{
			delete rule;
			return NULL;
		}

		pRule = rule;
	}
	else if ( sType.compare( "regexp", Qt::CaseInsensitive ) == 0 )
	{
		CRegExpRule* rule = new CRegExpRule();
		if ( !rule->parseContent( attributes.value( "content" ).toString() ) )
		{
			delete rule;
			return NULL;
		}

		pRule = rule;
	}
	else if ( sType.compare( "content", Qt::CaseInsensitive ) == 0 )
	{
		const QString sMatch = attributes.value( "match" ).toString();
		const QString sContent = attributes.value( "content" ).toString();

		const QString sUrn = sContent.left( 4 );

		if ( nVersion < 2.0 )
		{
			// This handles "old style" Shareaza RegExp rules.
			if ( sMatch.compare( "regexp", Qt::CaseInsensitive ) == 0 )
			{
				CRegExpRule* rule = new CRegExpRule();
				if ( !rule->parseContent( sContent ) )
				{
					delete rule;
					return NULL;
				}

				pRule = rule;
			}
			// This handles "old style" Shareaza hash rules.
			else if ( sUrn.compare( "urn:", Qt::CaseInsensitive ) == 0 )
			{
				CHashRule* rule = new CHashRule();
				if ( !rule->parseContent( sContent ) )
				{
					delete rule;
					return NULL;
				}

				pRule = rule;
			}
		}

		if ( !pRule )
		{
			bool all = ( sMatch.compare( "all", Qt::CaseInsensitive ) == 0 );

			if ( all || sMatch.compare( "any", Qt::CaseInsensitive ) == 0 )
			{
				CContentRule* rule = new CContentRule();
				if ( !rule->parseContent( sContent ) )
				{
					delete rule;
					return NULL;
				}

				rule->setAll( all );
				pRule = rule;
			}
			else
			{
				return NULL;
			}
		}
	}
#if SECURITY_ENABLE_GEOIP
	else if ( sType.compare( "country", Qt::CaseInsensitive ) == 0 )
	{
		CCountryRule* rule = new CCountryRule();
		if ( !rule->parseContent( attributes.value( "content" ).toString() ) )
		{
			delete rule;
			return NULL;
		}

		pRule = rule;
	}
#endif // SECURITY_ENABLE_GEOIP
	else
	{
		return NULL;
	}

	const QString sAction = attributes.value( "action" ).toString();

	if ( sAction.compare( "deny", Qt::CaseInsensitive ) == 0 || sAction.isEmpty() )
	{
		pRule->m_nAction = srDeny;
	}
	else if ( sAction.compare( "accept", Qt::CaseInsensitive ) == 0 )
	{
		pRule->m_nAction = srAccept;
	}
	else if ( sAction.compare( "null", Qt::CaseInsensitive ) == 0 )
	{
		pRule->m_nAction = srNull;
	}
	else
	{
		delete pRule;
		return NULL;
	}

	pRule->m_sComment = attributes.value( "comment" ).toString().trimmed();

	QString sExpire = attributes.value( "expire" ).toString();
	if ( sExpire.compare( "indefinite", Qt::CaseInsensitive ) == 0 )
	{
		pRule->m_tExpire = srIndefinite;
	}
	else if ( sExpire.compare( "session", Qt::CaseInsensitive ) == 0 )
	{
		pRule->m_tExpire = srSession;
	}
	else
	{
		pRule->m_tExpire = sExpire.toUInt();
	}

	QString sUUID = attributes.value( "uuid" ).toString();
	if ( sUUID.isEmpty() )
		sUUID = attributes.value( "guid" ).toString();

	if ( sUUID.isEmpty() )
	{
		pRule->m_oUUID = QUuid::createUuid();
	}
	else
	{
		pRule->m_oUUID = QUuid( sUUID );
	}

	return pRule;
}

// Contains default code for XML generation.
void CSecureRule::toXML(const CSecureRule& oRule, QXmlStreamWriter& oXMLdocument)
{
	QString sValue;

	// Write rule action to XML file.
	switch ( oRule.m_nAction )
	{
	case srNull:
		sValue = "null";
		break;
	case srAccept:
		sValue = "accept";
		break;
	case srDeny:
		sValue = "deny";
		break;
	default:
		Q_ASSERT( false );
	}
	oXMLdocument.writeAttribute( "action", sValue );


	// Write expiry date.
	if ( oRule.m_tExpire == srSession )
	{
		sValue = "session";
	}
	else if ( oRule.m_tExpire == srIndefinite )
	{
		sValue = "indefinite";
	}
	else if ( oRule.m_tExpire > srSession )
	{
		sValue = "%1";
		sValue.arg( oRule.m_tExpire );
	}
	oXMLdocument.writeAttribute( "expire", sValue );


	// Write rule UUID.
	sValue = oRule.m_oUUID.toString();
	oXMLdocument.writeAttribute( "uuid", sValue );


	// Write user comment.
	if ( !( oRule.m_sComment.isEmpty() ) )
	{
		oXMLdocument.writeAttribute( "comment", oRule.m_sComment );
	}
}

/**
 * @brief CSecureRule::isExpired allows to check whether a rule has expired.
 * @param tNow indicates the current time in seconds since 1.1.1970UTC
 * @param bSession indicates whether this is the end of the session/start of a new session. In both
 * cases, set this to true and the return value for session ban rules will be true.
 * @return true if the rule has expired, false otherwise
 */
bool CSecureRule::isExpired(quint32 tNow, bool bSession) const
{
	if ( m_tExpire == srIndefinite ) return false;
	if ( m_tExpire == srSession ) return bSession;
	return m_tExpire < tNow;
}

/**
 * @brief CSecureRule::getExpiryTime allows to access the expiry time of a rule.
 * @return srIndefinite = 0, srSession = 1 or the time in seconds since 1.1.1970UTC when the rule
 * will/has expire(d)
 */
quint32 CSecureRule::getExpiryTime() const
{
	return m_tExpire;
}

/**
 * @brief CSecureRule::count increases the total and today hit counters by one each.
 * Requires Locking: /
 */
void CSecureRule::count()
{
	m_nToday.fetchAndAddOrdered(1);
	m_nTotal.fetchAndAddOrdered(1);
}

/**
 * @brief CSecureRule::resetCount resets total and today hit counters to 0.
 * Requires Locking: /
 */
void CSecureRule::resetCount()
{
	m_nToday.fetchAndStoreOrdered(0);
	m_nTotal.fetchAndAddOrdered(0);
}

/**
 * @brief CSecureRule::getTodayCount allows to access the today hit counter.
 * @return the value of the today hit counter.
 * Requires Locking: /
 */
quint32 CSecureRule::getTodayCount() const
{
	return m_nToday.loadAcquire();
}

/**
 * @brief CSecureRule::getTotalCount allows to access the total hit counter.
 * @return the value of the total hit counter.
 * Requires Locking: /
 */
quint32 CSecureRule::getTotalCount() const
{
	return m_nTotal.loadAcquire();
}

/**
 * @brief CSecureRule::loadTotalCount allows to access the total hit counter.
 * @param nTotal the new value of the total hit counter.
 * Requires Locking: /
 */
void CSecureRule::loadTotalCount( quint32 nTotal )
{
	m_nTotal.storeRelease(nTotal);
}

/**
 * @brief CSecureRule::type allows to access the type of this rule.
 * @return the rule type.
 * Requires Locking: R
 */
CSecureRule::TRuleType CSecureRule::type() const
{
	return m_nType;
}

void CSecureRule::toXML( QXmlStreamWriter& ) const
{

}

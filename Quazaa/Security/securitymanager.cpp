/*
** $Id$
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

#include <math.h>

#include <QDateTime>
#include <QMetaType>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "securitymanager.h"

#include "geoiplist.h"
#include "quazaasettings.h"
#include "timedsignalqueue.h"
#include "Misc/timeoutwritelocker.h"

#include "debug_new.h"

Security::CSecurity securityManager;
using namespace Security;

/**
  * Constructor. Variable initializations.
  * Locking: /
  */
// Plz note: QApplication hasn't been started when the global definition creates this object,
// so no qt specific calls (for example connect() or emit signal may be used over here.
// See initialize() for that kind of initializations.
CSecurity::CSecurity() :
	m_sDataPath( "" ),
	m_bLogIPCheckHits( false ),
	m_tRuleExpiryInterval( 0 ),
	m_tMissCacheExpiryInterval( 0 ),
	m_bUseMissCache( false ),
	m_bIsLoading( false ),
	m_bNewRulesLoaded( false ),
	m_nPendingOperations( 0 ),
	m_bSaved( true ),
	m_bDenyPolicy( false )
{
}

/**
  * Destructor.
  * Locking: RW
  */
CSecurity::~CSecurity()
{
}

/**
  * Sets the deny policy to a given value.
  * Locking: RW
  */
void CSecurity::setDenyPolicy(bool bDenyPolicy)
{
	QWriteLocker l( &m_pRWLock );
	m_bDenyPolicy = bDenyPolicy;
}

/**
  * Checks whether a rule with the same UUID exists within the security database.
  * Locking: R
  */
bool CSecurity::check(const CSecureRule* const pRule) const
{
	QReadLocker l( &(securityManager.m_pRWLock) );
	return pRule != NULL && getUUID( pRule->m_oUUID ) != m_Rules.end();
}

//////////////////////////////////////////////////////////////////////
// CSecurity rule modification
/**
  * Adds a rule to the security database. This makes no copy of the rule, so don't delete it after adding.
  * Locking: RW
  */
void CSecurity::add(CSecureRule* pRule)
{
	if ( !pRule ) return;

	// check for invalid rules
	Q_ASSERT( pRule->type() > 0 && pRule->type() < 8 && pRule->m_nAction < 3 );
	Q_ASSERT( !pRule->m_oUUID.isNull() );

	CSecureRule::RuleType type = pRule->type();
	CSecureRule* pExRule = NULL;

	bool bNewAddress = false;
	bool bNewHit	 = false;

	QWriteLocker mutex( &m_pRWLock );

	// Special treatment for the different types of rules
	switch ( type )
	{
	case CSecureRule::srContentAddress:
	{
		uint nIP = qHash( ((CIPRule*)pRule)->IP() );
		CAddressRuleMap::iterator i = m_IPs.find( nIP );

		if ( i != m_IPs.end() ) // there is a potentially conflicting rule in our map
		{
			pExRule = (*i).second;
			if ( pExRule->m_oUUID   != pRule->m_oUUID   ||
				 pExRule->m_nAction != pRule->m_nAction ||
				 pExRule->m_tExpire != pRule->m_tExpire )
			{
				// remove conflicting rule if one of the important attributes
				// differs from the rule we'd like to add
				remove( pExRule, false );
			}
			else
			{
				// the rule does not need to be added.
				delete pRule;
				pRule = NULL;
				return;
			}

			pExRule = NULL;
		}

		m_IPs[ nIP ] = (CIPRule*)pRule;

		bNewAddress = true;

		if ( !m_bUseMissCache )
			evaluateCacheUsage();
	}
	break;

	case CSecureRule::srContentAddressRange:
	{
		CIPRangeRuleList::iterator i = m_IPRanges.begin();
		CIPRangeRule* pOldRule = NULL;

		while ( i != m_IPRanges.end() )
		{
			pOldRule = *i;
			if ( pOldRule->m_oUUID == pRule->m_oUUID )
			{
				if ( pOldRule->m_nAction != pRule->m_nAction ||
					 pOldRule->m_tExpire != pRule->m_tExpire ||
					 pOldRule->IP()      != ((CIPRangeRule*)pRule)->IP() ||
					 pOldRule->mask()    != ((CIPRangeRule*)pRule)->mask() )
				{
					// remove conflicting rule if one of the important attributes
					// differs from the rule we'd like to add
					remove( pOldRule, false );
				}
				else
				{
					// the rule does not need to be added.
					delete pRule;
					pRule = NULL;
					return;
				}
			}
			pOldRule = NULL;
			++i;
		}

		m_IPRanges.push_front( (CIPRangeRule*)pRule );

		bNewAddress = true;

		if ( !m_bUseMissCache )
			evaluateCacheUsage();
	}
	break;

	case CSecureRule::srContentCountry:
	{
		QString country = ((CCountryRule*)pRule)->getContentString();
		CCountryRuleMap::iterator i = m_Countries.find( country );

		bNewAddress = true;

		if ( i != m_Countries.end() ) // there is a potentially conflicting rule in our map
		{
			pExRule = (*i).second;
			if ( pExRule->m_oUUID   != pRule->m_oUUID   ||
				 pExRule->m_nAction != pRule->m_nAction ||
				 pExRule->m_tExpire != pRule->m_tExpire )
			{
				// remove conflicting rule if one of the important attributes
				// differs from the rule we'd like to add
				remove( pExRule, false );
			}
			else
			{
				// the rule does not need to be added.
				delete pRule;
				pRule = NULL;
				return;
			}

			pExRule = NULL;
		}

		m_Countries[ country ] = (CCountryRule*)pRule;

		bNewAddress = true;

		if ( !m_bUseMissCache )
			evaluateCacheUsage();
	}
	break;

	case CSecureRule::srContentHash:
	{
		CHashRule* pHashRule = (CHashRule*)pRule;

		QList< CHash > oHashes = pHashRule->getHashes();

		if ( oHashes.isEmpty() )
		{
			// There is no point in adding an invalid rule...
			delete pRule;
			pRule = NULL;
			return;
		}

		CIterator i = getHash( oHashes );

		if ( i != m_Rules.end() )
		{
			pExRule = *i;
			if ( pHashRule->hashEquals( *((CHashRule*)*i) ) )
			{
				if ( pExRule->m_oUUID   != pRule->m_oUUID   ||
					 pExRule->m_nAction != pRule->m_nAction ||
					 pExRule->m_tExpire != pRule->m_tExpire )
				{
					// remove conflicting rule if one of the important attributes
					// differs from the rule we'd like to add
					remove( pExRule, false );
				}
				else
				{
					// there is no point on adding a rule for the same content twice,
					// as that content is already blocked.
					delete pRule;
					pRule = NULL;
					return;
				}
			}
		}

		// If there isn't a rule for this content or there is a rule for
		// similar but not 100% identical content, add hashes to map.
		foreach ( CHash oHash, oHashes )
		{
			m_Hashes.insert( CHashPair( qHash( oHash.RawValue() ), pHashRule ) );
		}

		bNewHit	= true;
	}
	break;

	case CSecureRule::srContentRegExp:
	{
		CRegExpRuleList::iterator i = m_RegExpressions.begin();
		CRegExpRule* pOldRule = NULL;

		while ( i != m_RegExpressions.end() )
		{
			pOldRule = *i;
			if ( (*i)->m_oUUID == pRule->m_oUUID )
			{
				if ( pOldRule->m_nAction != pRule->m_nAction ||
					 pOldRule->m_tExpire != pRule->m_tExpire ||
					 pOldRule->getContentString() != ((CRegExpRule*)pRule)->getContentString() )
				{
					// remove conflicting rule if one of the important attributes
					// differs from the rule we'd like to add
					remove( pOldRule, false );
				}
				else
				{
					// the rule does not need to be added.
					delete pRule;
					pRule = NULL;
					return;
				}
			}
			pOldRule = NULL;
			++i;
		}

		m_RegExpressions.push_front( (CRegExpRule*)pRule );

		bNewHit	= true;
	}
	break;

	case CSecureRule::srContentText:
	{
		CContentRuleList::iterator i = m_Contents.begin();
		CContentRule* pOldRule = NULL;

		while ( i != m_Contents.end() )
		{
			pOldRule = *i;
			if ( (*i)->m_oUUID == pRule->m_oUUID )
			{
				if ( pOldRule->m_nAction != pRule->m_nAction ||
					 pOldRule->m_tExpire != pRule->m_tExpire ||
					 pOldRule->getContentString() != ((CRegExpRule*)pRule)->getContentString() )
				{
					// remove conflicting rule if one of the important attributes
					// differs from the rule we'd like to add
					remove( pOldRule, false );
				}
				else
				{
					// the rule does not need to be added.
					delete pRule;
					pRule = NULL;
					return;
				}
			}
			pOldRule = NULL;
			++i;
		}

		m_Contents.push_front( (CContentRule*)pRule );

		bNewHit	= true;
	}
	break;

	case CSecureRule::srContentUserAgent:
	{
		QString agent = ((CUserAgentRule*)pRule)->getContentString();
		CUserAgentRuleMap::iterator i = m_UserAgents.find( agent );

		if ( i != m_UserAgents.end() ) // there is a conflicting rule in our map
		{
			pExRule = (*i).second;
			if ( pExRule->m_oUUID   != pRule->m_oUUID   ||
				 pExRule->m_nAction != pRule->m_nAction ||
				 pExRule->m_tExpire != pRule->m_tExpire )
			{
				// remove conflicting rule if one of the important attributes
				// differs from the rule we'd like to add
				remove( pExRule, false );
			}
			else
			{
				// the rule does not need to be added.
				delete pRule;
				pRule = NULL;
				return;
			}

			pExRule = NULL;
		}

		m_UserAgents[ agent ] = (CUserAgentRule*)pRule;

		bNewHit	= true;
	}
	break;

	default:
		Q_ASSERT( false );
	}

	// Add rule to list of all rules
	CIterator iExRule = getUUID( pRule->m_oUUID );
	if ( iExRule != m_Rules.end() ) // we do not allow 2 rules by the same UUID
	{
		remove( iExRule );
	}
	m_Rules.push_back( pRule );

	// If an address rule is added, the miss cache is cleared either in whole or just the relevant address
	if ( type == CSecureRule::srContentAddress )
	{
		m_Cache.erase( qHash( ((CIPRule*)pRule)->IP() ) );

		if ( !m_bUseMissCache )
			evaluateCacheUsage();
	}
	else if ( type == CSecureRule::srContentAddressRange || type == CSecureRule::srContentCountry )
	{
		missCacheClear( true );

		if ( !m_bUseMissCache )
			evaluateCacheUsage();
	}

	if ( bNewAddress )	// only add IP, IP range and country rules to the queue
	{
		m_newAddressRules.push( pRule->getCopy() );
	}
	else if ( bNewHit )		// only add rules related to hit filtering to the queue
	{
		m_newHitRules.push( pRule->getCopy() );
	}

	m_bSaved = false;

	// Inform CSecurityTableModel about new rule.
	emit ruleAdded( pRule );

	// If we're not loading, check all lists for newly denied hosts.
	if ( !m_bIsLoading )
	{
		// Unlock mutex before performing system wide security check.
		mutex.unlock();
		// In case we are currently loading rules from file,
		// this is done uppon completion of the entire process.
		sanityCheck();
		save();
	}
}

/**
  * Frees all memory and storing containers. Removes all rules.
  * Locking: RW
  */
void CSecurity::clear()
{
	QWriteLocker l( &m_pRWLock );

	m_IPs.clear();
	m_IPRanges.clear();
	m_Hashes.clear();
	m_RegExpressions.clear();
	m_Contents.clear();
	m_UserAgents.clear();

	qDeleteAll( m_Rules );
	m_Rules.clear();

	qDeleteAll( m_loadedAddressRules );
	m_loadedAddressRules.clear();

	qDeleteAll( m_loadedHitRules );
	m_loadedHitRules.clear();

	CSecureRule* pRule = NULL;
	while( m_newAddressRules.size() )
	{
		pRule = m_newAddressRules.front();
		m_newAddressRules.pop();
		delete pRule;
	}

	while ( m_newHitRules.size() )
	{
		pRule = m_newHitRules.front();
		m_newHitRules.pop();
		delete pRule;
	}

	signalQueue.setInterval( m_idRuleExpiry, m_tRuleExpiryInterval );
	missCacheClear( true );

	m_bSaved = false;
}

//////////////////////////////////////////////////////////////////////
// CSecurity ban
/**
  * Bans a given IP for a specified amount of time and adds strComment as comment to the security rule.
  * If bMessage is set to true, a notification is send to the system log.
  * Locking: RW
  */
void CSecurity::ban(const QHostAddress& oAddress, BanLength nBanLength, bool bMessage, const QString& sComment)
{
	if ( oAddress.isNull() )
	{
		Q_ASSERT( false ); // if this happens, make sure to fix the caller... :)
		return;
	}

	QWriteLocker mutex( &m_pRWLock );

	quint32 tNow = getTNowUTC();

	CAddressRuleMap::const_iterator i = m_IPs.find( qHash( oAddress ) );
	if ( i != m_IPs.end() )
	{
		CSecureRule* pIPRule = (*i).second;

		if ( pIPRule->m_nAction == CSecureRule::srDeny )
		{
			if ( !( sComment.isEmpty() ) )
				pIPRule->m_sComment = sComment;

			switch( nBanLength )
			{
			case banSession:
				pIPRule->m_tExpire = CSecureRule::srSession;
				return;

			case ban5Mins:
				pIPRule->m_tExpire = tNow + 300;
				break;

			case ban30Mins:
				pIPRule->m_tExpire = tNow + 1800;
				break;

			case ban2Hours:
				pIPRule->m_tExpire = tNow + 7200;
				break;

			case banWeek:
				pIPRule->m_tExpire = tNow + 604800;
				break;

			case banMonth:
				pIPRule->m_tExpire = tNow + 2592000;
				break;

			case banForever:
				pIPRule->m_tExpire = CSecureRule::srIndefinite;
				return;

			default:
				Q_ASSERT( false );
			}

			if ( bMessage )
			{
				systemLog.postLog( LogSeverity::Security,
				                   tr( "Adjusted ban expiry time of %1 to %2." ).arg(
				                       oAddress.toString(),
				                       QDateTime::fromTime_t( pIPRule->m_tExpire ).toString() ) );
			}

			return;
		}
		else
		{
			remove( pIPRule, false );
		}
	}

	mutex.unlock();

	CIPRule* pIPRule = new CIPRule();

	switch( nBanLength )
	{
	case banSession:
		pIPRule->m_tExpire	= CSecureRule::srSession;
		pIPRule->m_sComment	= "Session Ban";
		break;
	case ban5Mins:
		pIPRule->m_tExpire	= tNow + 300;
		pIPRule->m_sComment	= "Temp Ignore";
		break;
	case ban30Mins:
		pIPRule->m_tExpire	= tNow + 1800;
		pIPRule->m_sComment	= "Temp Ignore";
		break;
	case ban2Hours:
		pIPRule->m_tExpire	= tNow + 7200;
		pIPRule->m_sComment	= "Temp Ignore";
		break;
	case banWeek:
		pIPRule->m_tExpire	= tNow + 604800;
		pIPRule->m_sComment	= "Client Block";
		break;
	case banMonth:
		pIPRule->m_tExpire	= tNow + 2592000; // 60*60*24*30 = 30 days
		pIPRule->m_sComment	= "Quick IP Block";
		break;
	case banForever:
		pIPRule->m_tExpire	= CSecureRule::srIndefinite;
		pIPRule->m_sComment	= "Ban";
		break;
	default:
		pIPRule->m_tExpire	= CSecureRule::srSession;
		pIPRule->m_sComment	= "Session Ban";
		Q_ASSERT( false ); // this should never happen
	}

	if ( !( sComment.isEmpty() ) )
		pIPRule->m_sComment = sComment;

	pIPRule->setIP( oAddress );

	add( pIPRule );

	if ( bMessage )
	{
		//		theApp.Message( MSG_NOTICE, IDS_NETWORK_SECURITY_BLOCKED,
		//			(LPCTSTR)FromIP( AF_INET, &oAddress ) );
	}
}

/**
  * Bans a given file for a specified amount of time and adds strComment as comment to the security rule.
  * If bMessage is set to true, a notification is send to the system log.
  * Locking: R + RW while adding
  */
// TODO: Implement priorization of hashes to not to ban too many hashes per file.
/*void CSecurity::ban(const CFile& oFile, BanLength nBanLength, bool bMessage, const QString& strComment)
{
	if ( oFile.isNull() )
	{
		// theApp.Message( MSG_ERROR, IDS_SECURITY_ERROR_FILE_BAN, "" );
		return;
	}

	QReadLocker mutex( &m_pRWLock );

	quint32 tNow = getTNowUTC();

	CIterator i = getHash( oFile.getHashes() );
	bool bAlreadyBlocked = ( i != m_Rules.end() );

	mutex.unlock();

	if ( bAlreadyBlocked )
	{
		if ( bMessage )
		{
			// theApp.Message( MSG_NOTICE, IDS_SECURITY_ERROR_FILE_ALREADY_BANNED, (LPCTSTR)pFile->m_sName );
		}
	}
	else
	{
		CHashRule* pRule = new CHashRule();

		switch ( nBanLength )
		{
		case banSession:
			pRule->m_tExpire	= CSecureRule::srSession;
			pRule->m_sComment	= "Session Ban";
			break;
		case ban5Mins:
			pRule->m_tExpire	= tNow + 300;
			pRule->m_sComment	= "Temp Ignore";
			break;
		case ban30Mins:
			pRule->m_tExpire	= tNow + 1800;
			pRule->m_sComment	= "Temp Ignore";
			break;
		case ban2Hours:
			pRule->m_tExpire	= tNow + 7200;
			pRule->m_sComment	= "Temp Ignore";
			break;
		case banWeek:
			pRule->m_tExpire	= tNow + 604800;
			pRule->m_sComment	= "Client Block";
			break;
		case banMonth:
			pRule->m_tExpire	= tNow + 2592000; // 60*60*24*30 = 30 days
			pRule->m_sComment	= "Quick IP Block";
			break;
		case banForever:
			pRule->m_tExpire 	= CSecureRule::srIndefinite;
			pRule->m_sComment	= "Ban";
			break;
		default:
			pRule->m_tExpire	= CSecureRule::srSession;
			pRule->m_sComment	= "Session Ban";
			Q_ASSERT( false ); // this should never happen
		}

		if ( !( strComment.isEmpty() ) )
			pRule->m_sComment = strComment;

		QList<CHash> hashes = oFile.getHashes();

		if ( hashes.isEmpty() )
		{
			// We got no valid hashes from that file.
			QString str = " (File: " + oFile.fileName() + ")";
			// theApp.Message( MSG_ERROR, IDS_SECURITY_ERROR_FILE_BAN, (LPCTSTR)str );
			return;
		}
		else
		{
			pRule->setHashes( hashes );
		}

		add( pRule );

		if ( bMessage )
		{
			// theApp.Message( MSG_NOTICE, IDS_NETWORK_SECURITY_BLOCKED,
			// (LPCTSTR)pFile->m_sName );
		}
	}
}*/

//////////////////////////////////////////////////////////////////////
// public CSecurity access checks
/**
  * Checks an IP against the list of loaded new security rules.
  * Locking: R
  */
bool CSecurity::isNewlyDenied(const QHostAddress& oAddress)
{
	if ( oAddress.isNull() )
		return false;

	CSecureRule* pRule = NULL;
	QReadLocker l( &m_pRWLock );

	// This should only be called if new rules have been loaded previously.
	Q_ASSERT( m_bNewRulesLoaded );

	if ( m_loadedAddressRules.empty() )
		return false;

	CIterator i = m_loadedAddressRules.begin();

	while ( i != m_loadedAddressRules.end() )
	{
		pRule = *i;

		if ( pRule->match( oAddress ) )
		{
			// the rules are new, so we don't need to check whether they are expired or not

			hit( pRule );

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}

		++i;
	}

	return false;
}

/**
  * Checks a hit against the list of loaded new security rules.
  * Locking: R
  */
bool CSecurity::isNewlyDenied(const CQueryHit* pHit, const QList<QString>& lQuery)
{
	if ( !pHit )
		return false;

	CSecureRule* pRule = NULL;
	QReadLocker l( &m_pRWLock );

	// This should only be called if new rules have been loaded previously.
	Q_ASSERT( m_bNewRulesLoaded );

	if ( m_loadedHitRules.empty() )
		return false;

	CIterator i = m_loadedHitRules.begin();

	while ( i != m_loadedHitRules.end() )
	{
		pRule = *i;

		if ( pRule->match( pHit ) || pRule->match( pHit->m_sDescriptiveName ) ||
			 pRule->match( lQuery, pHit->m_sDescriptiveName ) )
		{
			// The rules are new, so we don't need to check whether they are expired or not.

			hit( pRule );

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}

		++i;
	}

	return false;
}

/**
  * Checks an IP against the security database. Writes a message to the system log if LogIPCheckHits is true.
  * Locking: R (+ RW if new IP is added to miss cache)
  */
bool CSecurity::isDenied(const QHostAddress& oAddress, const QString& /*source*/)
{
	if ( oAddress.isNull() )
		return false;

	QReadLocker mutex( &m_pRWLock );

	quint32 tNow = getTNowUTC();

	// First, check the miss cache if the IP is not included in the list of rules.
	// If the address is in cache, it is a miss and no further lookup is needed.
	if ( m_bUseMissCache && m_Cache.count( qHash( oAddress ) ) )
	{
		if ( m_bLogIPCheckHits )
		{
			// theApp.Message( MSG_DEBUG, "Skipped  repeat IP security check for %s (%i IPs cached; Call source: %s)",
			// oAddress.toString(), m_Cache.size(), source );
		}

		return m_bDenyPolicy;
	}
	else if ( m_bLogIPCheckHits )
	{
		// theApp.Message( MSG_DEBUG, "Called first-time IP security check for %s (Call source: %s)"),
		// oAddress.toString(), source );
	}

	// Second, check the fast IP rules lookup map.
	CAddressRuleMap::const_iterator it_1;
	it_1 = m_IPs.find( qHash( oAddress ) );

	if ( it_1 != m_IPs.end() )
	{
		CIPRule* pIPRule = (*it_1).second;
		if ( !pIPRule->isExpired( tNow ) )
		{
			hit( pIPRule );

			if ( pIPRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pIPRule->m_nAction == CSecureRule::srDeny )
				return true;
			else
				Q_ASSERT( pIPRule->m_nAction == CSecureRule::srNull );
		}
	}

	// Third, look up the IP in our country rule map.
	CCountryRuleMap::const_iterator it_2;
	it_2 = m_Countries.find( GeoIP.findCountryCode( oAddress ) );

	if ( it_2 != m_Countries.end() )
	{
		CSecureRule* pCountryRule = (*it_2).second;
		if ( !pCountryRule->isExpired( tNow ) )
		{
			hit( pCountryRule );

			if ( pCountryRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pCountryRule->m_nAction == CSecureRule::srDeny )
				return true;
			else
				Q_ASSERT( pCountryRule->m_nAction == CSecureRule::srNull );
		}
	}

	// Fourth, check whether the IP is contained within one of the IP range rules.
	CIPRangeRuleList::const_iterator it_3 = m_IPRanges.begin();
	while ( it_3 != m_IPRanges.end() )
	{
		CIPRangeRule* pRule = *it_3;

		if ( pRule->match( oAddress ) && !pRule->isExpired( tNow ) )
		{
			hit( pRule );

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
			else
				Q_ASSERT( pRule->m_nAction == CSecureRule::srNull );
		}

		++it_3;
	}

	mutex.unlock();

	// If the IP is not within the rules (and we're using the cache),
	// add the IP to the miss cache.
	missCacheAdd( qHash( oAddress ) );

	// In this case, return our default policy
	return m_bDenyPolicy;
}

/**
  * Checks a hit against the security database. Expects a list of all search keywords in the same
  * order they have been entered in the edit box of the GUI. Note that if no keywords are passed or
  * the list is in the wrong order, this beaks RegEx compatibility.
  * Locking: R (3x)
  */
bool CSecurity::isDenied(const CQueryHit* const pHit, const QList<QString> &lQuery)
{
	return ( isDenied( pHit )								// test hashes, file size and extension
		  || isDenied( pHit->m_sDescriptiveName )			// test file name
		  || isDenied( lQuery, pHit->m_sDescriptiveName ) );// test regex
}

/**
  * Returns true if the remote computer is running a client that is breaking GPL, causing problems, etc.
  * We don't actually ban these clients, but we don't accept them as a leaf. Can still upload, though.
  * Locking: /
  */
bool CSecurity::isClientBad(const QString& sUserAgent) const
{
	// No user agent- assume bad
	if ( sUserAgent.isEmpty() )					return true;	// They allowed to connect but no searches were performed

	QString sSubStr;

	// Bad/unapproved versions of Shareaza
	// Really obsolete versions of Shareaza should be blocked. (they may have bad settings)
    if ( sUserAgent.startsWith( "shareaza", Qt::CaseInsensitive ) )
	{
		sSubStr = sUserAgent.mid( 8 );
		if ( sSubStr.startsWith( " 0."  ) )		return true;
		if ( sSubStr.startsWith( " 1."  ) )		return true;	// There can be some 1.x versions of the real Shareaza but most are fakes
		if ( sSubStr.startsWith( " 2.0" ) )		return true;	// There is also a Shareaza rip-off that identify as Shareaza 2.0.0.0 (The real Shareaza 2.0.0.0 is so old and bad)
        if ( sSubStr.startsWith( " 2.1" ) )		return true;    // Old version
        if ( sSubStr.startsWith( " 2.2" ) )		return true;    // Old version
        if ( sSubStr.startsWith( " 2.3" ) )		return true;    // Old version
        if ( sSubStr.startsWith( " 2.4" ) )		return true;    // Old version
        if ( sSubStr.startsWith( " 2.5" ) )		return true;    // Old version
		if ( sSubStr.startsWith( " 3.0" ) )		return true;
		if ( sSubStr.startsWith( " 3.1" ) )		return true;
		if ( sSubStr.startsWith( " 3.2" ) )		return true;
		if ( sSubStr.startsWith( " 3.3" ) )		return true;
		if ( sSubStr.startsWith( " 3.4" ) )		return true;
		if ( sSubStr.startsWith( " 6."  ) )		return true;
		if ( sSubStr.startsWith( " 7."  ) )		return true;
		if ( sSubStr.startsWith( " Pro" ) )		return true;

		return false;
	}

	// Dianlei: Shareaza rip-off
	// add only based on alpha code, need verification for others
    if ( sUserAgent.startsWith( "Dianlei", Qt::CaseInsensitive ) )
	{
		sSubStr = sUserAgent.mid( 7 );
		if ( sSubStr.startsWith( " 1." ) )		return true;
		if ( sSubStr.startsWith( " 0." ) )		return true;

		return false;
	}

	// BearShare
    if ( sUserAgent.startsWith( "BearShare", Qt::CaseInsensitive ) )
	{
		sSubStr = sUserAgent.mid( 9 );
		if ( sSubStr.startsWith( " Lite"  ) )	return true;
		if ( sSubStr.startsWith( " Pro"   ) )	return true;
		if ( sSubStr.startsWith( " MP3"   ) ) 	return true;	// GPL breaker
		if ( sSubStr.startsWith( " Music" ) ) 	return true;	// GPL breaker
		if ( sSubStr.startsWith( " 6."    ) ) 	return true;	// iMesh

		return false;
	}

	// Fastload.TV
    if ( sUserAgent.startsWith( "Fastload.TV", Qt::CaseInsensitive ) )				return true;

	// Fildelarprogram
    if ( sUserAgent.startsWith( "Fildelarprogram", Qt::CaseInsensitive ) )			return true;

	// Gnutella Turbo (Look into this client some more)
    if ( sUserAgent.startsWith( "Gnutella Turbo", Qt::CaseInsensitive ) )			return true;

	// Identified Shareaza Leecher Mod
    if ( sUserAgent.startsWith( "eMule mod (4)", Qt::CaseInsensitive ) )				return true;

	// iMesh
    if ( sUserAgent.startsWith( "iMesh", Qt::CaseInsensitive ) )						return true;

	// Mastermax File Sharing
    if ( sUserAgent.startsWith( "Mastermax File Sharing", Qt::CaseInsensitive ) )	return true;

	// Trilix
    if ( sUserAgent.startsWith( "Trilix", Qt::CaseInsensitive ) )					return true;

	// Wru
    if ( sUserAgent.startsWith( "Wru", Qt::CaseInsensitive ) )						return true; // bad GuncDNA based client

	// GPL breakers- Clients violating the GPL
	// See http://www.gnu.org/copyleft/gpl.html
	// Some other breakers outside the list

    if ( sUserAgent.startsWith( "C -3.0.1", Qt::CaseInsensitive ) )					return true;

    if ( sUserAgent.startsWith( "eTomi", Qt::CaseInsensitive ) )						return true; // outdated rip-off

    if ( sUserAgent.startsWith( "FreeTorrentViewer", Qt::CaseInsensitive ) )			return true; // Shareaza rip-off / GPL violator

    if ( sUserAgent.startsWith( "K-Lite", Qt::CaseInsensitive ) )					return true; // Is it bad?

    if ( sUserAgent.startsWith( "mxie", Qt::CaseInsensitive ) )						return true; // Leechers, do not allow to connect

    if ( sUserAgent.startsWith( "P2P Rocket", Qt::CaseInsensitive ) )				return true; // Shareaza rip-off / GPL violator

    if ( sUserAgent.startsWith( "SlingerX", Qt::CaseInsensitive ) )					return true; // Rip-off with bad tweaks

    if ( sUserAgent.startsWith( "vagaa", Qt::CaseInsensitive ) )						return true; // Not clear why it's bad

    if ( sUserAgent.startsWith( "WinMX", Qt::CaseInsensitive ) )						return true;

	// Unknown- Assume OK
	return false;
}

/**
  * Returns true for especially bad / leecher clients, as well as user defined agent blocks.
  * Locking: R
  */
bool CSecurity::isAgentBlocked(const QString& sUserAgent)
{
	// The remote computer didn't send a "User-Agent", or it sent whitespace
	// We don't like those.
	if ( sUserAgent.isEmpty() )									return true;

	// i2hub - leecher client. (Tested, does not upload)
    if ( sUserAgent.startsWith( "i2hub 2.0", Qt::CaseInsensitive ) )					return true;

	// foxy - leecher client. (Tested, does not upload)
	// having something like Authentication which is not defined on specification
    if ( sUserAgent.startsWith( "foxy", Qt::CaseInsensitive ) )						return true;

	// Check by content filter
	return isAgentDenied( sUserAgent );
}

/**
  * Returns true for blocked vendors.
  * Locking: /
  */
bool CSecurity::isVendorBlocked(const QString& sVendor) const
{
	// foxy - leecher client. (Tested, does not upload)
	// having something like Authentication which is not defined on specification
	if ( sVendor.startsWith( "foxy" ) ) return true;

	// Allow it
	return false;
}

//////////////////////////////////////////////////////////////////////
// CSecurity load and save
/**
  * Initializes signal/slot connections, pulls settings and sets up cleanup interval counters.
  * Locking: RW
  */
bool CSecurity::start()
{
	// Register QSharedPointer<CSecureRule> to allow using this type with queued signal/slot connections.
	qRegisterMetaType< QSharedPointer< CSecureRule > >("QSharedPointer<CSecureRule>");

	connect( &quazaaSettings, SIGNAL(securitySettingsChanged()), SLOT(settingsChanged()), Qt::QueuedConnection );

	// Pull settings from global database to local copy.
	settingsChanged();

	// Set up interval timed cleanup operations.
	m_idRuleExpiry = signalQueue.push( this, "expire", m_tRuleExpiryInterval, true );
	m_idMissCacheExpiry = signalQueue.push( this, "missCacheClear", m_tMissCacheExpiryInterval, true );

	return load(); // Load security rules from HDD
}

/**
  * Saves the rules to disk, disonnects signal/slot connections, frees memory
  * and clears up storage containers.
  * Call this to make the security manager ready for destruction
  * Locking: RW
  */
bool CSecurity::stop()
{
	disconnect( &quazaaSettings, SIGNAL(securitySettingsChanged()), this, SLOT(settingsChanged()) );

	bool bSaved = save( true );			// Save security rules to disk.
	clear();				// Release memory and free containers.

	signalQueue.pop( this );// Remove all cleanup intervall timers from the queue.

	return bSaved;
}

/**
  * Loads the rule database from the HDD.
  * Locking: RW
  */
bool CSecurity::load()
{
	QString sPath = common::getLocation( common::userDataFiles ) + "security.dat";

	if ( load( sPath ) )
	{
		return true;
	}
	else
	{
		sPath = sPath + "_backup";
		return load( sPath );
	}
}

/**
  * Private helper method for load()
  * Locking: RW
  */
bool CSecurity::load( QString sPath )
{
	QFile oFile( sPath );

	if ( ! oFile.open( QIODevice::ReadOnly ) )
		return false;

	CSecureRule* pRule = NULL;

	try
	{
		clear();

		QDataStream fsFile( &oFile );

		quint16 nVersion;
		fsFile >> nVersion;

		bool bDenyPolicy;
		fsFile >> bDenyPolicy;

		quint32 nCount;
		fsFile >> nCount;

		quint32 tNow = getTNowUTC();

		QWriteLocker mutex( &m_pRWLock );
		m_bDenyPolicy = bDenyPolicy;
		m_bIsLoading = true; // Prevent sanity check from being executed at each add() operation.
		mutex.unlock();

		while ( nCount > 0 )
		{
			CSecureRule::load( pRule, fsFile, nVersion );

			if ( pRule->isExpired( tNow, true ) )
			{
				delete pRule;
			}
			else
			{
				add( pRule );
			}

			pRule = NULL;

			nCount--;
		}

		mutex.relock();
		m_bIsLoading = false;
		mutex.unlock();

		// If necessary perform sanity check after loading.
		sanityCheck();
	}
	catch ( ... )
	{
		if ( pRule )
			delete pRule;

		clear();
		oFile.close();

		QWriteLocker l( &m_pRWLock );
		m_bIsLoading = false;

		return false;
	}
	oFile.close();

	return true;
}

/**
  * Helper method for save()
  * Requires Locking: R
  */
void CSecurity::writeToFile(const void * const pManager, QFile& oFile)
{
	quint16 nVersion = SECURITY_CODE_VERSION;

	QDataStream oStream( &oFile );
	CSecurity* pSManager = (CSecurity*)pManager;

	oStream << nVersion;
	oStream << pSManager->m_bDenyPolicy;
	oStream << pSManager->getCount();

	for ( CIterator i = pSManager->m_Rules.begin(); i != pSManager->m_Rules.end(); ++i )
	{
		const CSecureRule* pRule = *i;
		CSecureRule::save( pRule, oStream );
	}
}

/**
  * Saves the security rules to HDD. Skips saving if there haven't
  * been any important changes and bForceSaving is not set to true.
  * Locking: R (+RW - very short)
  */
bool CSecurity::save(bool bForceSaving) const
{
	m_pRWLock.lockForRead();

	if ( m_bSaved && !bForceSaving )		// Saving not required ATM.
	{
		m_pRWLock.unlock();
		return true;
	}

	if ( !common::securredSaveFile( common::userDataFiles, "security.dat", tr( "[Security] " ),
	                                this, &Security::CSecurity::writeToFile ) )
	{
		m_pRWLock.unlock();
		return false;
	}

	m_pRWLock.unlock();
	m_pRWLock.lockForWrite(); // temporary switch to write lock
	m_bSaved = true;

	m_pRWLock.unlock();
	return true;
}

//////////////////////////////////////////////////////////////////////
// CSecurity import
/**
  * Imports a security file with unknown format located at sPath into the security database.
  * Currently, only XML is being supported.
  * Locking: RW
  */
bool CSecurity::import(const QString &sPath)
{
	if ( fromXML( sPath ) )
	{
		sanityCheck();
		return true;
	}
	else
	{
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
// CSecurity XML
/**
  * Security xml file schema specification.
  */
const QString CSecurity::xmlns = "http://www.shareaza.com/schemas/Security.xsd";

/**
  * Exports all rules to an XML file.
  * Locking: R
  */
bool CSecurity::toXML(const QString& sPath) const
{
	QFile oFile( sPath );
	if( !oFile.open( QIODevice::ReadWrite ) )
		return false;

	QXmlStreamWriter xmlDocument( &oFile );

	xmlDocument.writeStartElement( xmlns, "security" );
	xmlDocument.writeAttribute( "version", "2.0" );

	// Once the security manager exits this method, m_pRWLock returns to its initial state.
	QReadLocker l( &m_pRWLock );

	for ( CIterator i = m_Rules.begin(); i != m_Rules.end() ; ++i )
	{
		(*i)->toXML( xmlDocument );
	}

	xmlDocument.writeEndElement();

	return true;
}

/**
  * Imports rules from an XML file.
  * Locking: RW
  */
bool CSecurity::fromXML(const QString& sPath)
{
	QFile oFile( sPath );
	if ( !oFile.open( QIODevice::ReadOnly ) )
		return false;

	QXmlStreamReader xmlDocument( &oFile );

	if ( xmlDocument.atEnd() ||
		 !xmlDocument.readNextStartElement() ||
		 xmlDocument.name().toString().compare( "security", Qt::CaseInsensitive ) != 0 )
		return false;

	float nVersion;

	QXmlStreamAttributes attributes = xmlDocument.attributes();
	{
		bool bOK;

		// attributes.value() returns an empty StringRef if the attribute "version" is not present.
		// In that case the conversion to float fails and version is set to 1.0.
		nVersion = attributes.value( "version" ).toString().toFloat( &bOK );
		if ( !bOK )
		{
			// theApp.Message( MSG_ERROR, IDS_SECURITY_XML_RULE_IMPORT_FAIL, xml version number error );
			nVersion = 1.0;
		}
	}

	const quint32 tNow = getTNowUTC();

	QWriteLocker mutex( &m_pRWLock );
	m_bIsLoading = true;
	mutex.unlock();

	CSecureRule* pRule = NULL;
	unsigned int nRuleCount = 0;

	// For all rules do:
	while ( !xmlDocument.atEnd() )
	{
		// Go forward until the beginning of the next rule
		xmlDocument.readNextStartElement();

		// Verify whether it's a rule
		if ( xmlDocument.name().toString().compare( "rule", Qt::CaseInsensitive ) != 0 )
		{
			// Parse it
			pRule = CSecureRule::fromXML( xmlDocument, nVersion );

			if ( pRule )
			{
				if ( getUUID( pRule->m_oUUID ) == m_Rules.end() && !pRule->isExpired( tNow ) )
				{
					add( pRule );
				}
				else
				{
					delete pRule;
				}
				pRule = NULL;
				nRuleCount++;
			}
			else
			{
				// theApp.Message( MSG_ERROR, IDS_SECURITY_XML_RULE_IMPORT_FAIL );
			}
		}
		else
		{
			// theApp.Message( MSG_ERROR, "unrecognized entry in xml file with name:", xmlDocument.name().toString() );
		}
	}

	mutex.relock();
	m_bIsLoading = false;

	return nRuleCount != 0;
}

const char* CSecurity::ruleInfoSignal = SIGNAL( ruleInfo( CSecureRule* ) );

/**
  * Returns the number of listeners to a given signal of the Security Manager.
  * Note that this is a method that violates the modularity principle.
  * Plz don't use it if you've got a problem with this (for example because of religious reasons).
  * Locking: /
  */
int CSecurity::receivers(const char* signal) const
{
	return QObject::receivers( signal );
}

//////////////////////////////////////////////////////////////////////
// Qt slots
/**
  * Qt slot. Triggers the Security Manager to emit all rules using the ruleInfo() signal.
  * Locking: R
  */
void CSecurity::requestRuleList()
{
	QReadLocker l( &m_pRWLock );
	for ( CIterator i = m_Rules.begin() ; i != m_Rules.end(); i++ )
	{
		emit ruleInfo( *i );
	}
}

//////////////////////////////////////////////////////////////////////
// Sanity checking slots
/**
  * Qt slot. Triggers a system wide sanity check.
  * The sanity check is delayed by 5s, if a write lock couldn't be aquired after 500ms.
  * The sanity check is aborted if it takes longer than 2min to finish.
  * Locking: RW
  */
void CSecurity::sanityCheck()
{
	bool bSuccess;
	CTimeoutWriteLocker( &m_pRWLock, bSuccess, 500 );

	quint32 tNow = getTNowUTC();

	if ( bSuccess )
	{
		// This indicates that an error happend previously.
		Q_ASSERT( !m_bNewRulesLoaded || !m_loadedAddressRules.empty() || !m_loadedHitRules.empty() );

		// Check whether there are new rules to deal with.
		bool bNewRules = !( m_newAddressRules.empty() && m_newHitRules.empty() );

		if ( bNewRules )
		{
			if ( !m_bNewRulesLoaded )
			{
				loadNewRules();

				// Failsafe mechanism in case there are massive problems somewhere else.
				signalQueue.push( this, SLOT(forceEndOfSanityCheck()), tNow + 120 );

				// Count how many "OK"s we need to get back.
				m_nPendingOperations = receivers( SIGNAL(performSanityCheck()) );

				// Inform all other modules aber the necessity of a sanity check.
				emit performSanityCheck();
			}
			else // other sanity check still in progress
			{
				// try again later
				signalQueue.push( this, SLOT(sanityCheck()), tNow + 5 );
			}
		}
	}
	else // We didn't get a write lock in a timely manner.
	{
		// try again later
		signalQueue.push( this, SLOT(sanityCheck()), tNow + 5 );
	}
}

/**
  * Qt slot. Must be notified by all listeners to the signal performSanityCheck().
  * Locking: RW
  */
void CSecurity::sanityCheckPerformed()
{
	bool bSuccess;
	CTimeoutWriteLocker( &m_pRWLock, bSuccess, 500 );

	if ( m_bNewRulesLoaded )
	{
		if ( bSuccess )
		{
			Q_ASSERT( m_nPendingOperations > 0 );

			if ( --m_nPendingOperations == 0 )
			{
				clearNewRules();
			}
		}
		else // we didn't get a lock
		{
			// try again later
			signalQueue.push( this, SLOT(sanityCheckPerformed()), getTNowUTC() + 2 );
		}
	}
}

/**
  * Qt slot. Aborts the currently running sanity check by clearing its rule lists.
  * Locking: RW
  */
void CSecurity::forceEndOfSanityCheck()
{
#ifdef _DEBUG
	if ( m_nPendingOperations )
	{
		qDebug() << "[Security] Error: Sanity check aborted. Most probable reason: It took some "
		         << "component longer than 2min to call sanityCheckPerformed() after having "
		         << "recieved the signal performSanityCheck().";
		Q_ASSERT( false );
	}
#endif //_DEBUG

	QWriteLocker l( &m_pRWLock );
	clearNewRules();
	m_nPendingOperations = 0;
}

/**
  * Qt slot. Checks the security database for expired rules.
  * Locking: RW
  */
void CSecurity::expire()
{
	qDebug() << "[Security] Expiring old rules now!";

	QWriteLocker l( &m_pRWLock );

	quint32 tNow = getTNowUTC();
	quint16 nCount = 0;

	CIterator j, i = m_Rules.begin();
	while (  i != m_Rules.end() )
	{
		if ( (*i)->isExpired( tNow ) )
		{
			j = i;
			++j; // Make sure we have a valid iterator after removal.
			remove( i );
			i = j;
			++nCount;
		}
		else
		{
			++i;
		}
	}

	qDebug() << "[Security]"
	         << QString::number( nCount ).toLocal8Bit().data()
	         << "Rules expired.";
}

/**
  * Qt slot. Clears the miss cache.
  * Locking: RW
  */
void CSecurity::missCacheClear()
{
	QWriteLocker l( &m_pRWLock );
	missCacheClear( false );
}

/**
  * Qt slot. Pulls all relevant settings from quazaaSettings.Security
  * and refreshes all components depending on them.
  * Locking: RW
  */
void CSecurity::settingsChanged()
{
	QWriteLocker l( &m_pRWLock );
	m_sDataPath					= quazaaSettings.Security.DataPath;
	m_bLogIPCheckHits			= quazaaSettings.Security.LogIPCheckHits;

	if ( m_tRuleExpiryInterval != quazaaSettings.Security.RuleExpiryInterval * 1000 )
	{
		m_tRuleExpiryInterval = quazaaSettings.Security.RuleExpiryInterval * 1000;
		signalQueue.setInterval( m_idRuleExpiry, m_tRuleExpiryInterval );
	}

	if ( m_tMissCacheExpiryInterval != quazaaSettings.Security.MissCacheExpiryInterval * 1000 )
	{
		m_tMissCacheExpiryInterval = quazaaSettings.Security.MissCacheExpiryInterval * 1000;
		signalQueue.setInterval( m_idMissCacheExpiry, m_tMissCacheExpiryInterval );
	}
}

//////////////////////////////////////////////////////////////////////
// Private method definitions

void CSecurity::loadNewRules()
{
	Q_ASSERT( !( m_loadedAddressRules.size() || m_loadedHitRules.size() ) );	// should both be empty
	Q_ASSERT( m_newAddressRules.size() || m_newHitRules.size() );		// there should be at least 1 new rule

	CSecureRule* pRule = NULL;

	while ( m_newAddressRules.size() )
	{
		pRule = m_newAddressRules.front();
		m_newAddressRules.pop();

		// Only IP, IP range and coutry rules are allowed.
		Q_ASSERT( pRule->type() != 0 && pRule->type() < 4 );

		m_loadedAddressRules.push_back( pRule );

		pRule = NULL;
	}

	while ( m_newHitRules.size() )
	{
		pRule = m_newHitRules.front();
		m_newHitRules.pop();

		// Only hit related rules are allowed.
		Q_ASSERT( pRule->type() > 3 );

		m_loadedHitRules.push_back( pRule );

		pRule = NULL;
	}

	m_bNewRulesLoaded = true;
}

void CSecurity::clearNewRules()
{
	Q_ASSERT( m_bNewRulesLoaded );
	Q_ASSERT( m_loadedAddressRules.size() || m_loadedHitRules.size() ); // Should contain at least one rule

	CSecureRule* pRule = NULL;

	while ( m_loadedAddressRules.size() )
	{
		pRule = m_loadedAddressRules.front();
		m_loadedAddressRules.pop_front();

		// only IP, IP range and coutry rules allowed
		Q_ASSERT( pRule->type() > 0 && pRule->type() < 4 );

		delete pRule;
		pRule = NULL;
	}

	while ( m_loadedHitRules.size() )
	{
		pRule = m_loadedHitRules.front();
		m_loadedHitRules.pop_front();

		// Only hit related rules are allowed
		Q_ASSERT( pRule->type() > 3 );

		delete pRule;
		pRule = NULL;
	}

	m_bNewRulesLoaded = false;
}

CSecurity::CIterator CSecurity::getHash(const QList< CHash >& hashes) const
{
	// We are not searching for any hash. :)
	if ( hashes.isEmpty() )
		return m_Rules.end();

	CHashRuleMap::const_iterator i;

	// For each hash that has been given to the function:
	foreach ( CHash oHash, hashes )
	{
		// 1. Check whether a corresponding rule can be found in our lookup container.
		i = m_Hashes.find( qHash( oHash.RawValue() ) );

		// 2. Iterate threw all rules that include the current hash
		// (this is important for weaker hashes to deal correctly with hash collisions)
		while ( i != m_Hashes.end() && (*i).first == qHash( oHash.RawValue() ) )
		{
			if ( (*i).second->match( hashes ) )
				return getUUID( (*i).second->m_oUUID );
			++i;
		}
	}

	return m_Rules.end();
}

CSecurity::CIterator CSecurity::getUUID(const QUuid& oUUID) const
{
	for ( CIterator i = m_Rules.begin() ; i != m_Rules.end(); i++ )
	{
		if ( (*i)->m_oUUID == oUUID ) return i;
	}

	return m_Rules.end();
}

void CSecurity::remove(CIterator it)
{
	if ( it == m_Rules.end() )
		return;

	CSecureRule* pRule = *it;
	CSecureRule::RuleType type = pRule->type();

	// Removing the rule from special containers for fast access.
	switch ( type )
	{
	case CSecureRule::srContentAddress:
	{
		uint nIP = qHash( ((CIPRule*)pRule)->IP() );
		CAddressRuleMap::iterator i = m_IPs.find( nIP );

		if ( i != m_IPs.end() && (*i).second->m_oUUID == pRule->m_oUUID )
		{
			m_IPs.erase( i );

			if ( m_bUseMissCache )
				evaluateCacheUsage();
		}
	}
	break;

	case CSecureRule::srContentAddressRange:
	{
		CIPRangeRuleList::iterator i = m_IPRanges.begin();

		while ( i != m_IPRanges.end() )
		{
			if ( (*i)->m_oUUID == pRule->m_oUUID )
			{
				m_IPRanges.erase( i );
				break;
			}

			++i;
		}

		if ( m_bUseMissCache )
			evaluateCacheUsage();
	}
	break;

	case CSecureRule::srContentCountry:
	{
		QString country = pRule->getContentString();
		CCountryRuleMap::iterator i = m_Countries.find( country );

		if ( i != m_Countries.end() && (*i).second->m_oUUID == pRule->m_oUUID )
		{
			m_Countries.erase( i );

			if ( m_bUseMissCache )
				evaluateCacheUsage();
		}
	}
	break;

	case CSecureRule::srContentHash:
	{
		CHashRule* pHashRule = (CHashRule*)pRule;

		QList< CHash > oHashes = pHashRule->getHashes();

		CHashRuleMap::iterator i;
		foreach ( CHash oHash, oHashes )
		{
			i = m_Hashes.find( qHash( oHash.RawValue() ) );

			while ( i != m_Hashes.end() && (*i).first == qHash( oHash.RawValue() ) )
			{
				if ( (*i).second->m_oUUID == pHashRule->m_oUUID )
				{
					m_Hashes.erase( i );
					break;
				}

				++i;
			}
		}
	}
	break;

	case CSecureRule::srContentText:
	{
		CContentRuleList::iterator i = m_Contents.begin();

		while ( i != m_Contents.end() )
		{
			if ( (*i)->m_oUUID == pRule->m_oUUID )
			{
				m_Contents.erase( i );
				break;
			}

			++i;
		}
	}
	break;

	case CSecureRule::srContentRegExp:
	{
		CRegExpRuleList::iterator i = m_RegExpressions.begin();

		while ( i != m_RegExpressions.end() )
		{
			if ( (*i)->m_oUUID == pRule->m_oUUID )
			{
				m_RegExpressions.erase( i );
				break;
			}

			++i;
		}
	}
	break;

	case CSecureRule::srContentUserAgent:
	{
		CRegExpRuleList::iterator i = m_RegExpressions.begin();

		while ( i != m_RegExpressions.end() )
		{
			if ( (*i)->m_oUUID == pRule->m_oUUID )
			{
				m_RegExpressions.erase( i );
				break;
			}

			++i;
		}
	}
	break;

	default:
		Q_ASSERT( false );
	}

	m_bSaved = false;

	// Remove rule entry from list of all rules
	// m_Rules.erase( common::getRWIterator<CSecurityRuleList>( m_Rules, it ) );
	m_Rules.erase( getRWIterator( it ) );

	emit ruleRemoved( QSharedPointer<CSecureRule>( pRule ) );
}

bool CSecurity::isAgentDenied(const QString& strUserAgent)
{
	if ( strUserAgent.isEmpty() )
		return false;

	quint32 tNow = getTNowUTC();

	QReadLocker lock( &m_pRWLock );

	CUserAgentRuleMap::iterator i = m_UserAgents.find( strUserAgent );
	if ( i != m_UserAgents.end() )
	{
		CUserAgentRule* pRule = (*i).second;

		if ( pRule->match( strUserAgent ) )
		{
			if ( pRule->isExpired( tNow ) )
			{
				// We need a different kind of lock here, so unlocking and relocking is required.
				lock.unlock();
				remove( pRule );
				lock.relock();
			}

			hit( pRule );

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}
	}

	return false;
}

void CSecurity::missCacheAdd(const uint &nIP)
{
	if ( m_bUseMissCache )
	{
		m_pRWLock.lockForWrite();
		m_Cache.insert( nIP );
		evaluateCacheUsage();
		m_pRWLock.unlock();
	}
}
void CSecurity::missCacheClear(bool bRefreshInterval)
{
	m_Cache.clear();

	if ( !m_bUseMissCache )
		evaluateCacheUsage();

	if ( bRefreshInterval )
		signalQueue.setInterval( m_idMissCacheExpiry, m_tMissCacheExpiryInterval );
}
void CSecurity::evaluateCacheUsage()
{
	double nCache		= m_Cache.size();
	double nIPMap		= m_IPs.size();
	double nCountryMap	= m_Countries.size();

	static const double log2	= log( 2.0 );

	static double s_nCache		= 0;
	static double s_nLogCache	= 0;

	static double s_nIPMap		= -1;
	static double s_nCountryMap	= 0;
	static double s_nLogMult	= 0;

	// Only do the heavy log operations if necessary.
	if ( s_nCache != nCache )
	{
		s_nCache    = nCache;

		if ( !nCache )
			nCache = 1;

		s_nLogCache = log( nCache );
	}

	// Only do the heavy log operations if necessary.
	if ( s_nIPMap != nIPMap || s_nCountryMap != nCountryMap )
	{
		s_nIPMap		= nIPMap;
		s_nCountryMap	= nCountryMap;

		if ( !nIPMap )
			nIPMap = 1;
		if ( !nCountryMap )
			nCountryMap = 1;

		s_nLogMult	= log( nIPMap * nCountryMap );
	}

	m_bUseMissCache = ( s_nLogCache < s_nLogMult + m_IPRanges.size() * log2 );
}

bool CSecurity::isDenied(const QString& sContent)
{
	if ( sContent.isEmpty() )
		return false;

	quint32 tNow = getTNowUTC();

	QReadLocker mutex( &m_pRWLock );

	CContentRuleList::iterator i = m_Contents.begin();
	while ( i != m_Contents.end() )
	{
		CContentRule* pRule = *i;

		if ( pRule->match( sContent ) )
		{
			if ( pRule->isExpired( tNow ) )
			{
//				// Make sure iterator stays valid after removal.
//				++i;

//				// We need a different kind of lock here, so unlocking and relocking is required.
//				mutex.unlock();
//				remove( pRule );
//				mutex.relock();

				continue; // let the expire() method handle expiries
			}

			hit( pRule );

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}

		++i;
	}

	return false;
}
bool CSecurity::isDenied(const CQueryHit* const pHit)
{
	if ( !pHit )
		return false;

	const QList<CHash>& lHashes = pHit->m_lHashes;

	quint32 tNow = getTNowUTC();

	QReadLocker mutex( &m_pRWLock );

	// Search for a rule matching these hashes
	CIterator it = getHash( lHashes );

	// If this rule matches the file, return the specified action.
	if ( it != m_Rules.end() )
	{
		CHashRule* pRule = ((CHashRule*)*it);
		if ( pRule->match( lHashes ) && !pRule->isExpired( tNow ) )
		{
			hit( pRule );

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}
	}

	// Else check other content rules.
	CContentRuleList::iterator i = m_Contents.begin();
	CContentRule* pRule;
	while ( i != m_Contents.end() )
	{
		pRule = *i;

		if ( pRule->match( pHit ) )
		{
			if ( pRule->isExpired( tNow ) )
			{
//				bool bBegin = i != m_Contents.begin();
//				if ( bBegin )
//				{
//					i--;
//				}

//				// We need a different kind of lock here, so unlocking and relocking is required.
//				mutex.unlock();
//				remove( pRule );
//				mutex.relock();

//				if ( !bBegin )
//				{
//					i = m_Contents.begin();
//				}
				continue; // let the expire() method handle expiries
			}

			hit( pRule );

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}

		++i;
	}

	return false;
}
bool CSecurity::isDenied(const QList<QString>& lQuery, const QString& sContent)
{
	if ( lQuery.isEmpty() || sContent.isEmpty() )
		return false;

	quint32 tNow = getTNowUTC();

	QReadLocker mutex( &m_pRWLock );

	CRegExpRuleList::iterator i = m_RegExpressions.begin();
	while ( i != m_RegExpressions.end() )
	{
		CRegExpRule* pRule = *i;

		if ( pRule->match( lQuery, sContent ) )
		{
			if ( pRule->isExpired( tNow ) )
			{
//				// Keep the iterator valid while removing the rule.
//				bool bBegin = ( i == m_RegExpressions.begin() );
//				if ( !bBegin )
//				{
//					i--;
//				}

//				// We need a different kind of lock here, so unlocking and relocking is required.
//				mutex.unlock();
//				remove( pRule );
//				mutex.relock();

//				if ( bBegin )
//				{
//					i = m_RegExpressions.begin();
//				}
				continue; // let the expire() method handle expiries
			}

			hit( pRule );

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}

		++i;
	}

	return false;
}


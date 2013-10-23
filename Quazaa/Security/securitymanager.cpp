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

#include <math.h>

#include <QDateTime>
#include <QMetaType>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "securitymanager.h"

#include "quazaasettings.h"
#include "timedsignalqueue.h"
#include "Misc/timeoutwritelocker.h"

#include "debug_new.h"

CSecurity securityManager;

/**
  * Constructor. Variable initializations.
  * Locking: /
  */
// QApplication hasn't been started when the global definition creates this object, so
// no qt specific calls (for example connect() or emit signal) may be used over here.
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
	m_nMaxUnsavedRules( 100 ),
	m_nUnsaved( 0 ),
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
  * Adds a rule to the security database. This makes no copy of the rule, so don't delete it after
  * adding.
  * Locking: RW
  */
void CSecurity::add(CSecureRule* pRule)
{
	if ( !pRule ) return;

	// check for invalid rules
	Q_ASSERT( pRule->type() > 0 && pRule->type() < 8 && pRule->m_nAction < 3 );
	Q_ASSERT( !pRule->m_oUUID.isNull() );

	CSecureRule::TRuleType type = pRule->type();
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
		TAddressRuleMap::iterator i = m_IPs.find( nIP );

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
		TIPRangeRuleList::iterator i = m_IPRanges.begin();
		CIPRangeRule* pOldRule = NULL;

		while ( i != m_IPRanges.end() )
		{
			pOldRule = *i;
			if ( pOldRule->m_oUUID == pRule->m_oUUID )
			{
				if ( pOldRule->m_nAction != pRule->m_nAction ||
					 pOldRule->m_tExpire != pRule->m_tExpire ||
					 pOldRule->startIP()      != ((CIPRangeRule*)pRule)->startIP() ||
					 pOldRule->endIP()    != ((CIPRangeRule*)pRule)->endIP() )
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
#if SECURITY_ENABLE_GEOIP
	case CSecureRule::srContentCountry:
	{
		QString country = ((CCountryRule*)pRule)->getContentString();
		TCountryRuleMap::iterator i = m_Countries.find( country );

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
#endif // SECURITY_ENABLE_GEOIP
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

		TConstIterator i = getHash( oHashes );

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
			m_Hashes.insert( THashPair( qHash( oHash.RawValue() ), pHashRule ) );
		}

		bNewHit	= true;
	}
	break;

	case CSecureRule::srContentRegExp:
	{
		TRegExpRuleList::iterator i = m_RegExpressions.begin();
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
		TContentRuleList::iterator i = m_Contents.begin();
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
		TUserAgentRuleMap::iterator i = m_UserAgents.find( agent );

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
#if SECURITY_ENABLE_GEOIP
		Q_ASSERT( false );
#else
		Q_ASSERT( type == CSecureRule::srContentCountry );
#endif // SECURITY_ENABLE_GEOIP
	}

	// Add rule to list of all rules
	TConstIterator iExRule = getUUID( pRule->m_oUUID );
	if ( iExRule != m_Rules.end() ) // we do not allow 2 rules by the same UUID
	{
		remove( iExRule );
	}
	m_Rules.push_back( pRule );

	// If an address rule is added, the miss cache is cleared either in whole or just the relevant
	// address.
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

	const quint32 tNow = common::getTNowUTC();
	// if ( indefinite or longer than 1,5h ) increase unsaved rules counter
	if ( !pRule->m_tExpire || pRule->m_tExpire - tNow > 60 * 90 )
	{
		m_nUnsaved.fetchAndAddRelaxed( 1 );
	}

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

	m_nUnsaved.fetchAndStoreRelaxed( 0 );
}

//////////////////////////////////////////////////////////////////////
// CSecurity ban
/**
  * Bans a given IP for a specified amount of time and adds strComment as comment to the security
  * rule. If bMessage is set to true, a notification is send to the system log.
  * Locking: RW
  */
void CSecurity::ban(const QHostAddress& oAddress, BanLength::TBanLength nBanLength, bool bMessage,
					const QString& sComment, bool bAutomatic)
{
	if ( oAddress.isNull() )
	{
		Q_ASSERT( false ); // if this happens, make sure to fix the caller... :)
		return;
	}

	QWriteLocker mutex( &m_pRWLock );

	const quint32 tNow = common::getTNowUTC();

	TAddressRuleMap::const_iterator i = m_IPs.find( qHash( oAddress ) );
	if ( i != m_IPs.end() )
	{
		CSecureRule* pIPRule = (*i).second;

		pIPRule->m_bAutomatic = bAutomatic;

		if ( pIPRule->m_nAction == CSecureRule::srDeny )
		{
			if ( !( sComment.isEmpty() ) )
				pIPRule->m_sComment = sComment;

			switch( nBanLength )
			{
			case BanLength::Session:
				pIPRule->m_tExpire = CSecureRule::srSession;
				return;

			case BanLength::FiveMinutes:
				pIPRule->m_tExpire = tNow + BanLength::FiveMinutes;
				break;

			case BanLength::ThirtyMinutes:
				pIPRule->m_tExpire = tNow + BanLength::ThirtyMinutes;
				break;

			case BanLength::TwoHours:
				pIPRule->m_tExpire = tNow + BanLength::TwoHours;
				break;

			case BanLength::SixHours:
				pIPRule->m_tExpire = tNow + BanLength::SixHours;
				break;

			case BanLength::TwelveHours:
				pIPRule->m_tExpire = tNow + BanLength::TwelveHours;
				break;

			case BanLength::Day:
				pIPRule->m_tExpire = tNow + BanLength::Day;
				break;

			case BanLength::Week:
				pIPRule->m_tExpire = tNow + BanLength::Week;
				break;

			case BanLength::Month:
				pIPRule->m_tExpire = tNow + BanLength::Month;
				break;

			case BanLength::Forever:
				pIPRule->m_tExpire = CSecureRule::srIndefinite;
				return;

			default:
				Q_ASSERT( false );
			}

			if ( bMessage )
			{
				postLog( LogSeverity::Security,
						 tr( "Adjusted ban expiry time of %1 to %2."
							 ).arg( oAddress.toString(),
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

	pIPRule->m_bAutomatic = bAutomatic;

	switch( nBanLength )
	{
	case BanLength::Session:
		pIPRule->m_tExpire  = CSecureRule::srSession;
		pIPRule->m_sComment = tr( "Session Ban" );
		break;

	case BanLength::FiveMinutes:
		pIPRule->m_tExpire  = tNow + BanLength::FiveMinutes;
		pIPRule->m_sComment = tr( "Temp Ignore (5 min)" );
		break;

	case BanLength::ThirtyMinutes:
		pIPRule->m_tExpire  = tNow + BanLength::ThirtyMinutes;
		pIPRule->m_sComment = tr( "Temp Ignore (30 min)" );
		break;

	case BanLength::TwoHours:
		pIPRule->m_tExpire  = tNow + BanLength::TwoHours;
		pIPRule->m_sComment = tr( "Temp Ignore (2 h)" );
		break;

	case BanLength::SixHours:
		pIPRule->m_tExpire  = tNow + BanLength::SixHours;
		pIPRule->m_sComment = tr( "Temp Ignore (2 h)" );
		break;

	case BanLength::TwelveHours:
		pIPRule->m_tExpire  = tNow + BanLength::TwelveHours;
		pIPRule->m_sComment = tr( "Temp Ignore (2 h)" );
		break;

	case BanLength::Day:
		pIPRule->m_tExpire  = tNow + BanLength::Day;
		pIPRule->m_sComment = tr( "Temp Ignore (1 d)" );
		break;

	case BanLength::Week:
		pIPRule->m_tExpire  = tNow + BanLength::Week;
		pIPRule->m_sComment = tr( "Client Block (1 week)" );
		break;

	case BanLength::Month:
		pIPRule->m_tExpire  = tNow + BanLength::Month;
		pIPRule->m_sComment = tr( "Quick IP Block (1 month)" );
		break;

	case BanLength::Forever:
		pIPRule->m_tExpire  = CSecureRule::srIndefinite;
		pIPRule->m_sComment = tr( "Indefinite Ban" );
		break;

	default:
		pIPRule->m_tExpire  = CSecureRule::srSession;
		pIPRule->m_sComment = tr( "Session Ban" );
		Q_ASSERT( false ); // this should never happen
	}

	if ( !( sComment.isEmpty() ) )
		pIPRule->m_sComment = sComment;

	pIPRule->setIP( oAddress );

	add( pIPRule );

	if ( bMessage )
	{
		postLog( LogSeverity::Security,
				 tr( "Banned %1 until %2."
					 ).arg( oAddress.toString(),
							QDateTime::fromTime_t( pIPRule->m_tExpire ).toString() ) );
	}
}

/**
  * Bans a given file for a specified amount of time and adds strComment as comment to the security
  * rule. If bMessage is set to true, a notification is send to the system log.
  * Locking: R + RW while adding
  */
// TODO: Implement priorization of hashes to not to ban too many hashes per file.
/*void CSecurity::ban(const CFile& oFile, TBanLength nBanLength, bool bMessage, const QString& sComment)
{
	if ( oFile.isNull() )
	{
		postLog( LogSeverity::Security, tr( "Error: Could not ban invalid file." ) );
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
			postLog( LogSeverity::Security, tr( "Error: Could not ban already banned file." ) );
		}
	}
	else
	{
		CHashRule* pRule = new CHashRule();

		switch ( nBanLength )
		{
		case banSession:
			pIPRule->m_tExpire  = CSecureRule::srSession;
			pIPRule->m_sComment = tr( "Session Ban" );
			break;

		case ban5Mins:
			pIPRule->m_tExpire  = tNow + 300;
			pIPRule->m_sComment = tr( "Temp Ignore (5 min)" );
			break;

		case ban30Mins:
			pIPRule->m_tExpire  = tNow + 1800;
			pIPRule->m_sComment = tr( "Temp Ignore (30 min)" );
			break;

		case ban2Hours:
			pIPRule->m_tExpire  = tNow + 7200;
			pIPRule->m_sComment = tr( "Temp Ignore (2 h)" );
			break;

		case ban1Day:
			pIPRule->m_tExpire  = tNow + 86400;
			pIPRule->m_sComment = tr( "Temp Ignore (1 d)" );
			break;

		case banWeek:
			pIPRule->m_tExpire  = tNow + 604800;  // 60*60*24 = 1 day
			pIPRule->m_sComment = tr( "Client Block (1 week)" );
			break;

		case banMonth:
			pIPRule->m_tExpire  = tNow + 2592000; // 60*60*24*30 = 30 days
			pIPRule->m_sComment = tr( "Quick IP Block (1 month)" );
			break;

		case banForever:
			pIPRule->m_tExpire  = CSecureRule::srIndefinite;
			pIPRule->m_sComment = tr( "Indefinite Ban" );
			break;

		default:
			pIPRule->m_tExpire  = CSecureRule::srSession;
			pIPRule->m_sComment = tr( "Session Ban" );
			Q_ASSERT( false ); // this should never happen
		}

		if ( !( sComment.isEmpty() ) )
			pRule->m_sComment = sComment;

		QList<CHash> hashes = oFile.getHashes();

		if ( hashes.isEmpty() )
		{
			// We got no valid hashes from that file.
			QString str = " (File: " + oFile.fileName() + ")";
			postLog( LogSeverity::Security, tr( "Error: Could not ban file:" ) + str );
			return;
		}
		else
		{
			pRule->setHashes( hashes );
		}

		add( pRule );

		if ( bMessage )
		{
			postLog( LogSeverity::Security, tr( "Banned file: " ) + pFile->m_sName );
		}
	}
}*/

//////////////////////////////////////////////////////////////////////
// public CSecurity access checks
/**
  * Checks an IP against the list of loaded new security rules.
  * Locking: R
  */
bool CSecurity::isNewlyDenied(const CEndPoint& oAddress)
{
	if ( oAddress.isNull() )
		return false;

	CSecureRule* pRule = NULL;
	QReadLocker l( &m_pRWLock );

	// This should only be called if new rules have been loaded previously.
	Q_ASSERT( m_bNewRulesLoaded );

	if ( m_loadedAddressRules.empty() )
		return false;

	TConstIterator i = m_loadedAddressRules.begin();

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

	TConstIterator i = m_loadedHitRules.begin();

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
  * Checks an IP against the security database. Writes a message to the system log if LogIPCheckHits
  * is true.
  * Locking: R (+ RW while/if new IP is added to miss cache)
  */
bool CSecurity::isDenied(const CEndPoint &oAddress)
{
	if ( oAddress.isNull() )
		return false;

	QReadLocker mutex( &m_pRWLock );

	const quint32 tNow = common::getTNowUTC();

	// First, check the miss cache if the IP is not included in the list of rules.
	// If the address is in cache, it is a miss and no further lookup is needed.
	if ( m_bUseMissCache && m_Cache.count( qHash( oAddress ) ) )
	{
		if ( m_bLogIPCheckHits )
		{
			postLog( LogSeverity::Security,
					 tr( "Skipped repeat IP security check for %s (%i IPs cached"
						 ).arg( oAddress.toString(), (int)m_Cache.size() )
//			         + tr( "; Call source: %s" ).arg( source )
					 + tr( ")" ));
		}

		return m_bDenyPolicy;
	}
	else if ( m_bLogIPCheckHits )
	{
		postLog( LogSeverity::Security,
				 tr( "Called first-time IP security check for %s"
					 ).arg( oAddress.toString() )
//					+ tr( " ( Call source: %s)" ).arg( source )
				 );
	}

	// Second, if quazaa local/private blocking is turned on, check if the IP is local/private
	if( quazaaSettings.Security.IgnorePrivateIP )
	{
		if(isPrivate( oAddress ))
		{
			postLog( LogSeverity::Security, tr( "Local/Private IP denied: %s" ).arg( oAddress.toString() ) );
			return true;
		}
	}

	// Third, check whether the IP is contained within one of the IP range rules.
	TIPRangeRuleList::const_iterator it_3 = m_IPRanges.begin();
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

	// Fourth, check the fast IP rules lookup map.
	TAddressRuleMap::const_iterator it_1;
	it_1 = m_IPs.find( qHash( oAddress ) );

	if ( it_1 != m_IPs.end() )
	{
		CIPRule* pIPRule = (*it_1).second;
		if ( !pIPRule->isExpired( tNow ) && pIPRule->match( oAddress ) )
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

	// Fifth, look up the IP in our country rule map.
#if SECURITY_ENABLE_GEOIP
	TCountryRuleMap::const_iterator it_2;
	it_2 = m_Countries.find( oAddress.country() );

	if ( it_2 != m_Countries.end() )
	{
		CSecureRule* pCountryRule = (*it_2).second;
		if ( !pCountryRule->isExpired( tNow ) && pCountryRule->match( oAddress ) )
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
#endif // SECURITY_ENABLE_GEOIP

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
	return ( isDenied( pHit ) ||                             // test hashes, file size and extension
			 isDenied( pHit->m_sDescriptiveName ) ||         // test file name
			 isDenied( lQuery, pHit->m_sDescriptiveName ) ); // test regex
}

bool CSecurity::isPrivate(const CEndPoint &oAddress)
{
	if( oAddress.toIPv4Address() <= CEndPoint("0.255.255.255").toIPv4Address() )
		return true;

	if( oAddress.toIPv4Address() >= CEndPoint("10.0.0.0").toIPv4Address() &&
	  oAddress.toIPv4Address() <= CEndPoint("10.255.255.255").toIPv4Address() )
		return true;

	if( oAddress.toIPv4Address() >= CEndPoint("100.64.0.0").toIPv4Address() &&
	  oAddress.toIPv4Address() <= CEndPoint("100.127.255.255").toIPv4Address() )
		return true;

	if( oAddress.toIPv4Address() >= CEndPoint("127.0.0.0").toIPv4Address() &&
	  oAddress.toIPv4Address() <= CEndPoint("127.255.255.255").toIPv4Address() )
		return true;

	if( oAddress.toIPv4Address() >= CEndPoint("169.254.0.0").toIPv4Address() &&
	  oAddress.toIPv4Address() <= CEndPoint("169.254.255.255").toIPv4Address() )
		return true;

	if( oAddress.toIPv4Address() >= CEndPoint("172.16.0.0").toIPv4Address() &&
	  oAddress.toIPv4Address() <= CEndPoint("172.31.255.255").toIPv4Address() )
		return true;

	if( oAddress.toIPv4Address() >= CEndPoint("192.0.0.0").toIPv4Address() &&
	  oAddress.toIPv4Address() <= CEndPoint("192.0.2.255").toIPv4Address() )
		return true;

	if( oAddress.toIPv4Address() >= CEndPoint("192.0.2.0").toIPv4Address() &&
	  oAddress.toIPv4Address() <= CEndPoint("10.255.255.255").toIPv4Address() )
		return true;

	if( oAddress.toIPv4Address() >= CEndPoint("192.168.0.0").toIPv4Address() &&
	  oAddress.toIPv4Address() <= CEndPoint("192.168.255.255").toIPv4Address() )
		return true;

	if( oAddress.toIPv4Address() >= CEndPoint("198.18.0.0").toIPv4Address() &&
	  oAddress.toIPv4Address() <= CEndPoint("198.19.255.255").toIPv4Address() )
		return true;

	if( oAddress.toIPv4Address() >= CEndPoint("198.51.100.0").toIPv4Address() &&
	  oAddress.toIPv4Address() <= CEndPoint("198.51.100.255").toIPv4Address() )
		return true;

	if( oAddress.toIPv4Address() >= CEndPoint("203.0.113.0").toIPv4Address() &&
	  oAddress.toIPv4Address() <= CEndPoint("203.0.113.255").toIPv4Address() )
		return true;

	if( oAddress.toIPv4Address() >= CEndPoint("240.0.0.0").toIPv4Address() &&
	  oAddress.toIPv4Address() <= CEndPoint("255.255.255.255").toIPv4Address() )
		return true;

	return false;
}

/**
  * Returns true if the remote computer is running a client that is breaking GPL, causing problems,
  * etc. We don't actually ban these clients, but we don't accept them as a leaf. Can still upload,
  * though.
  * Locking: /
  */
bool CSecurity::isClientBad(const QString& sUserAgent) const
{
	// No user agent- assume bad - They allowed to connect but no searches were performed
	if ( sUserAgent.isEmpty() )                 return true;

	QString sSubStr;

	// Bad/unapproved versions of Shareaza
	// Really obsolete versions of Shareaza should be blocked. (they may have bad settings)
	if ( sUserAgent.startsWith( "shareaza", Qt::CaseInsensitive ) )
	{
		sSubStr = sUserAgent.mid( 8 );
		if ( sSubStr.startsWith( " 0."  ) )     return true;
		// There can be some 1.x versions of the real Shareaza but most are fakes
		if ( sSubStr.startsWith( " 1."  ) )     return true;
		// There is also a Shareaza rip-off that identifies as Shareaza 2.0.0.0. Also, the real
		if ( sSubStr.startsWith( " 2.0" ) )     return true; // Shareaza 2.0.0.0 is old and bad.
		if ( sSubStr.startsWith( " 2.1" ) )     return true; // Old version
		if ( sSubStr.startsWith( " 2.2" ) )     return true; // Old version
		if ( sSubStr.startsWith( " 2.3" ) )     return true; // Old version
		if ( sSubStr.startsWith( " 2.4" ) )     return true; // Old version
		if ( sSubStr.startsWith( " 2.5" ) )     return true; // Old version
		if ( sSubStr.startsWith( " 3.0" ) )     return true;
		if ( sSubStr.startsWith( " 3.1" ) )     return true;
		if ( sSubStr.startsWith( " 3.2" ) )     return true;
		if ( sSubStr.startsWith( " 3.3" ) )     return true;
		if ( sSubStr.startsWith( " 3.4" ) )     return true;
		if ( sSubStr.startsWith( " 6."  ) )     return true;
		if ( sSubStr.startsWith( " 7."  ) )     return true;
		if ( sSubStr.startsWith( " Pro" ) )     return true;

		return false;
	}

	// Dianlei: Shareaza rip-off
	// add only based on alpha code, need verification for others
	if ( sUserAgent.startsWith( "Dianlei", Qt::CaseInsensitive ) )
	{
		sSubStr = sUserAgent.mid( 7 );
		if ( sSubStr.startsWith( " 1." ) )      return true;
		if ( sSubStr.startsWith( " 0." ) )      return true;

		return false;
	}

	// BearShare
	if ( sUserAgent.startsWith( "BearShare", Qt::CaseInsensitive ) )
	{
		sSubStr = sUserAgent.mid( 9 );
		if ( sSubStr.startsWith( " Lite"  ) )   return true;
		if ( sSubStr.startsWith( " Pro"   ) )   return true;
		if ( sSubStr.startsWith( " MP3"   ) )   return true;	// GPL breaker
		if ( sSubStr.startsWith( " Music" ) )   return true;	// GPL breaker
		if ( sSubStr.startsWith( " 6."    ) )   return true;	// iMesh

		return false;
	}

	// Fastload.TV
	if ( sUserAgent.startsWith( "Fastload.TV", Qt::CaseInsensitive ) )            return true;

	// Fildelarprogram
	if ( sUserAgent.startsWith( "Fildelarprogram", Qt::CaseInsensitive ) )        return true;

	// Gnutella Turbo (Look into this client some more)
	if ( sUserAgent.startsWith( "Gnutella Turbo", Qt::CaseInsensitive ) )         return true;

	// Identified Shareaza Leecher Mod
	if ( sUserAgent.startsWith( "eMule mod (4)", Qt::CaseInsensitive ) )          return true;

	// iMesh
	if ( sUserAgent.startsWith( "iMesh", Qt::CaseInsensitive ) )                  return true;

	// Mastermax File Sharing
	if ( sUserAgent.startsWith( "Mastermax File Sharing", Qt::CaseInsensitive ) ) return true;

	// Trilix
	if ( sUserAgent.startsWith( "Trilix", Qt::CaseInsensitive ) )                 return true;

	// Wru (bad GuncDNA based client)
	if ( sUserAgent.startsWith( "Wru", Qt::CaseInsensitive ) )                    return true;

	// GPL breakers- Clients violating the GPL
	// See http://www.gnu.org/copyleft/gpl.html
	// Some other breakers outside the list

	if ( sUserAgent.startsWith( "C -3.0.1", Qt::CaseInsensitive ) )               return true;

	// outdated rip-off
	if ( sUserAgent.startsWith( "eTomi", Qt::CaseInsensitive ) )                  return true;

	// Shareaza rip-off / GPL violator
	if ( sUserAgent.startsWith( "FreeTorrentViewer", Qt::CaseInsensitive ) )      return true;

	// Is it bad?
	if ( sUserAgent.startsWith( "K-Lite", Qt::CaseInsensitive ) )                 return true;

	// Leechers, do not allow to connect
	if ( sUserAgent.startsWith( "mxie", Qt::CaseInsensitive ) )                   return true;

	// Shareaza rip-off / GPL violator
	if ( sUserAgent.startsWith( "P2P Rocket", Qt::CaseInsensitive ) )             return true;

	// Rip-off with bad tweaks
	if ( sUserAgent.startsWith( "SlingerX", Qt::CaseInsensitive ) )               return true;

	// Not clear why it's bad
	if ( sUserAgent.startsWith( "vagaa", Qt::CaseInsensitive ) )                  return true;

	if ( sUserAgent.startsWith( "WinMX", Qt::CaseInsensitive ) )                  return true;

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
	if ( sUserAgent.isEmpty() )                                         return true;

	// i2hub - leecher client. (Tested, does not upload)
	if ( sUserAgent.startsWith( "i2hub 2.0", Qt::CaseInsensitive ) )    return true;

	// foxy - leecher client. (Tested, does not upload)
	// having something like Authentication which is not defined on specification
	if ( sUserAgent.startsWith( "foxy", Qt::CaseInsensitive ) )         return true;

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
	if ( sVendor.startsWith( "foxy", Qt::CaseInsensitive ) ) return true;

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
	// We don't really need a lock here as nobody is supposed to use the manager before
	// it is properly initialized.
	m_sMessage = tr( "[Security] " );

	// Register QSharedPointer<CSecureRule> to allow using this type with queued signal/slot
	// connections.
	qRegisterMetaType< QSharedPointer< CSecureRule > >( "QSharedPointer<CSecureRule>" );

	connect( &quazaaSettings, SIGNAL( securitySettingsChanged() ), SLOT( settingsChanged() ),
			 Qt::QueuedConnection );

	// Pull settings from global database to local copy.
	settingsChanged();

	// Set up interval timed cleanup operations.
	m_idRuleExpiry = signalQueue.push( this, "expire", m_tRuleExpiryInterval, true );
	m_idMissCacheExpiry = signalQueue.push( this, "missCacheClear",
											m_tMissCacheExpiryInterval, true );

	return load(); // Load security rules from HDD.
}

/**
  * Saves the rules to disk, disonnects signal/slot connections, frees memory
  * and clears up storage containers.
  * Call this to make the security manager ready for destruction
  * Locking: RW
  */
bool CSecurity::stop()
{
	disconnect( &quazaaSettings, SIGNAL( securitySettingsChanged() ),
				this, SLOT( settingsChanged() ) );

	bool bSaved = save( true ); // Save security rules to disk.
	clear();                    // Release memory and free containers.

	signalQueue.pop( this );    // Remove all cleanup intervall timers from the queue.

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

		const quint32 tNow = common::getTNowUTC();

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
quint32 CSecurity::writeToFile(const void * const pManager, QFile& oFile)
{
	quint16 nVersion = SECURITY_CODE_VERSION;

	QDataStream oStream( &oFile );
	CSecurity* pSManager = (CSecurity*)pManager;

	oStream << nVersion;
	oStream << pSManager->m_bDenyPolicy;
	oStream << pSManager->getCount();

	for ( TConstIterator i = pSManager->m_Rules.begin(); i != pSManager->m_Rules.end(); ++i )
	{
		const CSecureRule* pRule = *i;
		CSecureRule::save( pRule, oStream );
	}

	return pSManager->getCount();
}

/**
  * Saves the security rules to HDD. Skips saving if there haven't
  * been any important changes and bForceSaving is not set to true.
  * Locking: R
  */
bool CSecurity::save(bool bForceSaving) const
{
	if ( m_nUnsaved.loadAcquire() < m_nMaxUnsavedRules && !bForceSaving )
	{
		return true;		// Saving not required ATM.
	}

	bool bReturn;
	m_pRWLock.lockForRead();

	if ( !common::securredSaveFile( common::userDataFiles, "security.dat", m_sMessage,
									this, &CSecurity::writeToFile ) )
	{
		bReturn = false;
	}
	else
	{
		m_nUnsaved.fetchAndStoreOrdered( 0 );
		bReturn = true;
	}

	m_pRWLock.unlock();
	return bReturn;
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

	for ( TConstIterator i = m_Rules.begin(); i != m_Rules.end() ; ++i )
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
		 xmlDocument.name().toString().compare( "security", Qt::CaseInsensitive ) )
		return false;

	postLog( LogSeverity::Information, tr( "Importing security rules from file: " ) + sPath );

	float nVersion;

	QXmlStreamAttributes attributes = xmlDocument.attributes();
	{
		bool bOK;
		QString sVersion;

		// attributes.value() returns an empty StringRef if the attribute "version" is not present.
		// In that case the conversion to float fails and version is set to 1.0.
		sVersion = attributes.value( "version" ).toString();
		nVersion = sVersion.toFloat( &bOK );
		if ( !bOK )
		{
			postLog( LogSeverity::Error,
					 tr( "Failed to read the Security XML version number from file." ) );
			nVersion = 1.0;
		}
	}

	const quint32 tNow = common::getTNowUTC();

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
		if ( xmlDocument.name().toString().compare( "rule", Qt::CaseInsensitive ) )
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
				++nRuleCount;
			}
			else
			{
				postLog( LogSeverity::Error, tr( "Failed to read a Security Rule from XML." ) );
			}
		}
		else
		{
			postLog( LogSeverity::Error,
					 tr( "Unrecognized entry in XML file with name: " ) +
					 xmlDocument.name().toString() );
		}
	}

	mutex.relock();
	m_bIsLoading = false;

	postLog( LogSeverity::Information, QString::number( nRuleCount ) + tr( " Rules imported." ) );

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
	for ( TConstIterator i = m_Rules.begin() ; i != m_Rules.end(); ++i )
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

	const quint32 tNow = common::getTNowUTC();

	if ( bSuccess )
	{
		// This indicates that an error happend previously.
		Q_ASSERT( !m_bNewRulesLoaded || !m_loadedAddressRules.empty() || !m_loadedHitRules.empty());

		// Check whether there are new rules to deal with.
		bool bNewRules = !( m_newAddressRules.empty() && m_newHitRules.empty() );

		if ( bNewRules )
		{
			if ( !m_bNewRulesLoaded )
			{
				loadNewRules();

				// Failsafe mechanism in case there are massive problems somewhere else.
				signalQueue.push( this, SLOT( forceEndOfSanityCheck() ), tNow + 120 );

				// Count how many "OK"s we need to get back.
				m_nPendingOperations = receivers( SIGNAL( performSanityCheck() ) );

				// Inform all other modules aber the necessity of a sanity check.
				emit performSanityCheck();
			}
			else // other sanity check still in progress
			{
				// try again later
				signalQueue.push( this, SLOT( sanityCheck() ), tNow + 5 );
			}
		}
	}
	else // We didn't get a write lock in a timely manner.
	{
		// try again later
		signalQueue.push( this, SLOT( sanityCheck() ), tNow + 5 );
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

	if ( bSuccess )
	{
		Q_ASSERT( m_bNewRulesLoaded );        // TODO: remove after testing
		Q_ASSERT( m_nPendingOperations > 0 );

		if ( --m_nPendingOperations == 0 )
		{
			postLog( LogSeverity::Debug, QString( "Sanity Check finished successfully. " ) +
					 QString( "Starting cleanup now." ), true );

			clearNewRules();
		}
		else
		{
			postLog( LogSeverity::Debug, QString( "A component finished with sanity checking. " ) +
					 QString( "Still waiting for %s other components to finish."
							  ).arg( m_nPendingOperations ), true );
		}
	}
	else // we didn't get a lock
	{
		// try again later
		signalQueue.push( this, SLOT( sanityCheckPerformed() ), common::getTNowUTC() + 2 );
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
		QString sTmp = QString( "Sanity check aborted. Most probable reason: It took some " ) +
					   QString( "component longer than 2min to call sanityCheckPerformed() " ) +
					   QString( "after having recieved the signal performSanityCheck()." );
		postLog( LogSeverity::Error, sTmp, true );
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
	postLog( LogSeverity::Debug, QString( "Expiring old rules now!" ), true );

	QWriteLocker l( &m_pRWLock );

	const quint32 tNow = common::getTNowUTC();
	quint16 nCount = 0;

	TConstIterator j, i = m_Rules.begin();
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

	postLog( LogSeverity::Debug, QString::number( nCount ) + " Rules expired.", true );
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

	// TODO: load from settings.
	m_nMaxUnsavedRules = 100;
}

//////////////////////////////////////////////////////////////////////
// Private method definitions

void CSecurity::loadNewRules()
{
	// should both be empty
	Q_ASSERT( !( m_loadedAddressRules.size() || m_loadedHitRules.size() ) );

	// there should be at least 1 new rule
	Q_ASSERT( m_newAddressRules.size() || m_newHitRules.size() );

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

	// There should at least be one rule.
	Q_ASSERT( m_loadedAddressRules.size() || m_loadedHitRules.size() );

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

CSecurity::TConstIterator CSecurity::getHash(const QList< CHash >& hashes) const
{
	// We are not searching for any hash. :)
	if ( hashes.isEmpty() )
		return m_Rules.end();

	THashRuleMap::const_iterator i;

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

CSecurity::TConstIterator CSecurity::getUUID(const QUuid& oUUID) const
{
	for ( TConstIterator i = m_Rules.begin() ; i != m_Rules.end(); i++ )
	{
		if ( (*i)->m_oUUID == oUUID ) return i;
	}

	return m_Rules.end();
}

/** Requires locking: yes */
void CSecurity::remove(TConstIterator it)
{
	if ( it == m_Rules.end() )
		return;

	CSecureRule* pRule = *it;

	// Removing the rule from special containers for fast access.
	switch ( pRule->type() )
	{
	case CSecureRule::srContentAddress:
	{
		uint nIP = qHash( ((CIPRule*)pRule)->IP() );
		TAddressRuleMap::iterator i = m_IPs.find( nIP );

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
		TIPRangeRuleList::iterator i = m_IPRanges.begin();

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

#if SECURITY_ENABLE_GEOIP
	case CSecureRule::srContentCountry:
	{
		QString country = pRule->getContentString();
		TCountryRuleMap::iterator i = m_Countries.find( country );

		if ( i != m_Countries.end() && (*i).second->m_oUUID == pRule->m_oUUID )
		{
			m_Countries.erase( i );

			if ( m_bUseMissCache )
				evaluateCacheUsage();
		}
	}
	break;
#endif // SECURITY_ENABLE_GEOIP

	case CSecureRule::srContentHash:
	{
		CHashRule* pHashRule = (CHashRule*)pRule;

		QList< CHash > oHashes = pHashRule->getHashes();

		THashRuleMap::iterator i;
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
		TContentRuleList::iterator i = m_Contents.begin();

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
		TRegExpRuleList::iterator i = m_RegExpressions.begin();

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
		TUserAgentRuleMap::iterator i = m_UserAgents.begin();

		while ( i != m_UserAgents.end() )
		{
			CUserAgentRule* pIRule = (*i).second;

			if ( pIRule->m_oUUID == pRule->m_oUUID )
			{
				m_UserAgents.erase( i );
				break;
			}

			++i;
		}
	}
	break;

	default:
#if SECURITY_ENABLE_GEOIP
		Q_ASSERT( false );
#else
		Q_ASSERT( pRule->type() == CSecureRule::srContentCountry );
#endif // SECURITY_ENABLE_GEOIP
	}

	m_nUnsaved.fetchAndAddRelaxed( 1 );

	// Remove rule entry from list of all rules
	// m_Rules.erase( common::getRWIterator<CSecurityRuleList>( m_Rules, it ) );
	m_Rules.erase( getRWIterator( it ) );

	emit ruleRemoved( QSharedPointer<CSecureRule>( pRule ) );
}

bool CSecurity::isAgentDenied(const QString& sUserAgent)
{
	if ( sUserAgent.isEmpty() )
		return false;

	const quint32 tNow = common::getTNowUTC();

	QReadLocker lock( &m_pRWLock );

	TUserAgentRuleMap::iterator i = m_UserAgents.find( sUserAgent );
	if ( i != m_UserAgents.end() )
	{
		CUserAgentRule* pRule = (*i).second;

		if ( !pRule->isExpired( tNow ) && pRule->match( sUserAgent ) )
		{
			hit( pRule );

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}
	}

	i = m_UserAgents.begin();
	CUserAgentRule* pRule;
	while ( i != m_UserAgents.end() )
	{
		pRule = (*i).second;
		++i;

		if ( !pRule->isExpired( tNow ) && pRule->partialMatch( sUserAgent ) )
		{
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
		// Make sure not to wait longer than 100ms for lock.
		if ( m_pRWLock.tryLockForWrite( 100 ) )
		{
			m_Cache.insert( nIP );
			evaluateCacheUsage();
			m_pRWLock.unlock();
		}
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

#if SECURITY_ENABLE_GEOIP
	double nCountryMap	= m_Countries.size();
#endif // SECURITY_ENABLE_GEOIP

	static const double log2	= log( 2.0 );

	static double s_nCache		= 0;
	static double s_nLogCache	= 0;

	static double s_nIPMap		= -1;
	static double s_nLogMult	= 0;

#if SECURITY_ENABLE_GEOIP
	static double s_nCountryMap	= 0;
#endif // SECURITY_ENABLE_GEOIP

	// Only do the heavy log operations if necessary.
	if ( s_nCache != nCache )
	{
		s_nCache    = nCache;

		if ( !nCache )
			nCache = 1;

		s_nLogCache = log( nCache );
	}

	// Only do the heavy log operations if necessary.
	if ( s_nIPMap != nIPMap
#if SECURITY_ENABLE_GEOIP
		 || s_nCountryMap != nCountryMap
#endif // SECURITY_ENABLE_GEOIP
		 )
	{
		s_nIPMap		= nIPMap;
#if SECURITY_ENABLE_GEOIP
		s_nCountryMap	= nCountryMap;
#endif // SECURITY_ENABLE_GEOIP

		if ( !nIPMap )
			nIPMap = 1;
#if SECURITY_ENABLE_GEOIP
		if ( !nCountryMap )
			nCountryMap = 1;
#endif // SECURITY_ENABLE_GEOIP

		s_nLogMult	= log( nIPMap
#if SECURITY_ENABLE_GEOIP
						   * nCountryMap
#endif // SECURITY_ENABLE_GEOIP
						   );
	}

	m_bUseMissCache = ( s_nLogCache < s_nLogMult + m_IPRanges.size() * log2 );
}

bool CSecurity::isDenied(const QString& sContent)
{
	if ( sContent.isEmpty() )
		return false;

	const quint32 tNow = common::getTNowUTC();

	QReadLocker mutex( &m_pRWLock );

	TContentRuleList::iterator i = m_Contents.begin();
	while ( i != m_Contents.end() )
	{
		CContentRule* pRule = *i;
		++i;

		if ( pRule->isExpired( tNow ) )
		{
			continue; // let the expire() method handle expiries
		}

		if ( pRule->match( sContent ) )
		{
			hit( pRule );

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}
	}

	return false;
}
bool CSecurity::isDenied(const CQueryHit* const pHit)
{
	if ( !pHit )
		return false;

	const QList<CHash>& lHashes = pHit->m_lHashes;

	const quint32 tNow = common::getTNowUTC();

	QReadLocker mutex( &m_pRWLock );

	// Search for a rule matching these hashes
	TConstIterator it = getHash( lHashes );

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
	TContentRuleList::iterator i = m_Contents.begin();
	CContentRule* pRule;
	while ( i != m_Contents.end() )
	{
		pRule = *i;
		++i;

		if ( pRule->isExpired( tNow ) )
		{
			continue; // let the expire() method handle expiries
		}

		if ( pRule->match( pHit ) )
		{
			hit( pRule );

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}
	}

	return false;
}
bool CSecurity::isDenied(const QList<QString>& lQuery, const QString& sContent)
{
	if ( lQuery.isEmpty() || sContent.isEmpty() )
		return false;

	const quint32 tNow = common::getTNowUTC();

	QReadLocker mutex( &m_pRWLock );

	TRegExpRuleList::iterator i = m_RegExpressions.begin();
	while ( i != m_RegExpressions.end() )
	{
		CRegExpRule* pRule = *i;
		++i;

		if ( pRule->isExpired( tNow ) )
		{
			continue; // let the expire() method handle expiries
		}

		if ( pRule->match( lQuery, sContent ) )
		{
			hit( pRule );

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}
	}

	return false;
}

/**
 * @brief postLog writes a message to the system log or to the debug output.
 * Requires locking: /
 * @param severity
 * @param message
 * @param bDebug Defaults to false. If set to true, the message is send  to qDebug() instead of
 * to the system log.
 */
void CSecurity::postLog(LogSeverity::Severity severity, QString message, bool bDebug)
{
	QString sMessage = securityManager.m_sMessage;

	switch ( severity )
	{
	case LogSeverity::Warning:
		sMessage += tr ( "Warning: " );
		break;

	case LogSeverity::Error:
		sMessage += tr ( "Error: " );
		break;

	case LogSeverity::Critical:
		sMessage += tr ( "Critical Error: " );
		break;

	default:
		break; // do nothing
	}

	sMessage += message;

	if ( bDebug )
	{
		qDebug() << sMessage.toLocal8Bit().constData();
	}
	else
	{
		systemLog.postLog( severity, sMessage );
	}
}

#include <math.h>

#include <QFile>

#include "security.h"
#include "quazaasettings.h"

using namespace security;
CSecurity Security;

// Plz note: QApplication hasn't been started when the global definition creates this object,
// so no qt specific calls (for example connect() or emit signal may be used over here.
CSecurity::CSecurity()
{
	m_bDenyPolicy = false;
	m_bIsLoading  = false;
	m_bNewRulesLoaded = false;
	m_bDelayedSanityCheckRequested = false;

	m_nLastRuleExpiryCheck = static_cast< quint32 >( time( NULL ) );
	m_nLastMissCacheExpiryCheck	= m_nLastRuleExpiryCheck;

	m_bSaved = true;
	m_bUseMissCache = false;
}

CSecurity::~CSecurity()
{
	clear();
}

void CSecurity::setDenyPolicy(bool bDenyPolicy)
{
	QMutexLocker l( &m_pSection );

	if( m_bDenyPolicy != bDenyPolicy )
	{
		m_bDenyPolicy = bDenyPolicy;
	}
}

bool CSecurity::check(CSecureRule* pRule)
{
	QMutexLocker l( &m_pSection );

	return pRule != NULL && getUUID( pRule->m_oUUID ) != getEnd();
}

//////////////////////////////////////////////////////////////////////
// CSecurity rule modification
void CSecurity::add(CSecureRule* pRule)
{
	if ( !pRule ) return;

	// check for invalid rules
	Q_ASSERT( pRule->getType() > 0 && pRule->getType() < 9 && pRule->m_nAction < 3 );

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	CSecureRule::RuleType type = pRule->getType();
	CSecureRule* pExRule = NULL;

	bool bNewAddress = false;
	bool bNewHit	 = false;

	QMutexLocker mutex( &m_pSection );

	// Add rule to list of all rules
	CIterator iExRule = getUUID( pRule->m_oUUID );
	if ( iExRule != getEnd() ) // we do not allow 2 rules by the same UUID
	{
		remove( iExRule );
	}
	m_Rules.push_back( pRule );

	// Special treatment for the different types of rules
	switch ( type )
	{
	case CSecureRule::srContentAddress:
		{
			QString sIP = ((CIPRule*)pRule)->getIP().toString();
			CAddressRuleMap::iterator i = m_IPs.find( sIP );

			if ( i != m_IPs.end() ) // there is a potentially conflicting rule in our map
			{
				pExRule = (*i).second;
				if ( pExRule->m_oUUID   != pRule->m_oUUID   ||
					 pExRule->m_nAction != pRule->m_nAction ||
					 pExRule->m_tExpire != pRule->m_tExpire )
				{
					// remove conflicting rule if one of the important attributes
					// differs from the rule we'd like to add
					mutex.unlock();
					remove( pExRule );
					mutex.relock();
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

			m_IPs[ sIP ] = (CIPRule*)pRule;

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
						 pOldRule->getIP()   != ((CIPRangeRule*)pRule)->getIP() ||
						 pOldRule->getMask() != ((CIPRangeRule*)pRule)->getMask() )
					{
						// remove conflicting rule if one of the important attributes
						// differs from the rule we'd like to add
						mutex.unlock();
						remove( pOldRule );
						mutex.relock();
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
				i++;
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
					mutex.unlock();
					remove( pExRule );
					mutex.relock();
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

			QString sSHA1 = pHashRule->getSHA1urn();
			QString sED2K = pHashRule->getED2Kurn();
			QString sBTIH = pHashRule->getBTIHurn();
			QString sTTH  = pHashRule->getTTHurn();
			QString sMD5  = pHashRule->getMD5urn();

			CIterator i = getHash( sSHA1, sED2K, sTTH, sBTIH, sMD5 );

			if ( i != getEnd() )
			{
				pExRule = *i;
				if ( ((CHashRule*)*i)->getHashWeight() == pHashRule->getHashWeight() )
				{
					if ( pExRule->m_oUUID   != pRule->m_oUUID   ||
						 pExRule->m_nAction != pRule->m_nAction ||
						 pExRule->m_tExpire != pRule->m_tExpire )
					{
						// remove conflicting rule if one of the important attributes
						// differs from the rule we'd like to add
						mutex.unlock();
						remove( pExRule );
						mutex.relock();
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
			if ( !( sSHA1.isEmpty() ) )
				m_Hashes.insert( CHashPair( sSHA1, pHashRule ) );

			if ( !( sED2K.isEmpty() ) )
				m_Hashes.insert( CHashPair( sED2K, pHashRule ) );

			if ( !( sBTIH.isEmpty() ) )
				m_Hashes.insert( CHashPair( sBTIH, pHashRule ) );

			if ( !( sTTH.isEmpty() ) )
				m_Hashes.insert( CHashPair( sTTH, pHashRule ) );

			if ( !( sMD5.isEmpty() ) )
				m_Hashes.insert( CHashPair( sMD5, pHashRule ) );

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
						mutex.unlock();
						remove( pOldRule );
						mutex.relock();
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
				i++;
			}

			m_RegExpressions.push_front( (CRegExpRule*)pRule );

			bNewHit	= true;
		}
		break;

	case CSecureRule::srContentAll:
	case CSecureRule::srContentAny:
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
						mutex.unlock();
						remove( pOldRule );
						mutex.relock();
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
				i++;
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
					mutex.unlock();
					remove( pExRule );
					mutex.relock();
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

	// If an address rule is added, the miss cache is cleared either in whole or just the relevant address
	if ( type == CSecureRule::srContentAddress )
	{
		m_Cache.erase( ((CIPRule*)pRule)->getIP().toString() );

		if ( !m_bUseMissCache )
			evaluateCacheUsage();
	}
	else if ( type == CSecureRule::srContentAddressRange || type == CSecureRule::srContentCountry )
	{
		missCacheClear( tNow );

		if ( !m_bUseMissCache )
			evaluateCacheUsage();
	}

	if ( bNewAddress )	// only add IP, IP range and country rules to the queue
		m_newAddressRules.push( pRule->getCopy() );

	if ( bNewHit )		// only add rules related to hit filtering to the queue
		m_newHitRules.push( pRule->getCopy() );

	m_bSaved = false;

	// Inform CSecurityTableModel about new rule.
	emit ruleAdded( pRule );

	// Check all lists for newly denied hosts
	if ( !m_bIsLoading )
	{
		// Unlock mutex before performing system wide security check.
		mutex.unlock();
		// In case we are currently loading rules from file,
		// this is done uppon completion of the entire process.
		sanityCheck();
	}
}

void CSecurity::remove(CSecureRule* pRule)
{
	if ( !pRule ) return;

	QMutexLocker l( &m_pSection );

	CIterator i = getUUID( pRule->m_oUUID );
	remove( i );
}

void CSecurity::clear()
{
	QMutexLocker mutex( &m_pSection );

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

	m_nLastRuleExpiryCheck = static_cast< quint32 >( time( NULL ) );
	missCacheClear( m_nLastRuleExpiryCheck );

	m_bSaved = false;
}

void CSecurity::expire()
{
	QMutexLocker mutex( &m_pSection );

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	CIterator i = getIterator();
	while (  i != getEnd() )
	{
		if ( (*i)->isExpired( tNow ) )
		{
			i = remove( i );
		}
		else
		{
			i++;
		}
	}

	m_nLastRuleExpiryCheck = tNow;
}

void CSecurity::sanityCheck(bool bDelay)
{
	QMutexLocker mutex( &m_pSection );

	if ( m_bNewRulesLoaded && m_loadedAddressRules.empty() && m_loadedHitRules.empty() )
	{
		m_bNewRulesLoaded = false;
		Q_ASSERT( false );	// This indicates that an error happend previously.
	}

	bool bNewRules = !( m_newAddressRules.empty() && m_newHitRules.empty() );

	m_bDelayedSanityCheckRequested = ( bDelay || ( m_bNewRulesLoaded && bNewRules ) );

	if ( !m_bDelayedSanityCheckRequested && bNewRules )
	{
		// Unlock mutex to prevent the security manager to be unavailable during the entire sanity check.
		mutex.unlock();

		// todo: change this to inform the application to perform a system wide sanity check
		bool bRequest = true; // = !PostMainWndMessage( WM_SANITY_CHECK );

		// Relock mutex again before changing member variables.
		mutex.relock();
		m_bDelayedSanityCheckRequested = bRequest;
	}

	if ( m_bDelayedSanityCheckRequested )
		m_nSanityCheckRequestTime = static_cast< quint32 >( time( NULL ) );
}

bool CSecurity::loadNewRules()
{
	// If we cannot aquire a lock for 200ms, return false.
	if ( !m_pSection.tryLock( 200 ) )
		return false;

	// We do not load new rules into this list if the currenty loaded ones have not been applied.
	if ( m_bNewRulesLoaded )
		return false;

	Q_ASSERT( !( m_loadedAddressRules.size() || m_loadedHitRules.size() ) );	// should both be empty
	Q_ASSERT( m_newAddressRules.size() || m_newHitRules.size() );		// there should be at least 1 new rule

	CSecureRule* pRule = NULL;

	while ( m_newAddressRules.size() )
	{
		pRule = m_newAddressRules.front();
		m_newAddressRules.pop();

		// Only IP, IP range and coutry rules are allowed.
		Q_ASSERT( pRule->getType() != 0 && pRule->getType() < 4 );

		m_loadedAddressRules.push_back( pRule );

		pRule = NULL;
	}

	while ( m_newHitRules.size() )
	{
		pRule = m_newHitRules.front();
		m_newHitRules.pop();

		// Only hit related rules are allowed.
		Q_ASSERT( pRule->getType() > 3 );

		m_loadedHitRules.push_back( pRule );

		pRule = NULL;
	}

	m_bNewRulesLoaded = true;

	// Unlocking is necessary as we're not using QMutexLocker.
	m_pSection.unlock();

	return true;
}

bool CSecurity::clearNewRules()
{
	QMutexLocker l( &m_pSection );

	if ( !m_bNewRulesLoaded )
		return false;

	Q_ASSERT( m_loadedAddressRules.size() || m_loadedHitRules.size() ); // Should contain at least one rule

	CSecureRule* pRule = NULL;

	while ( m_loadedAddressRules.size() )
	{
		pRule = m_loadedAddressRules.front();
		m_loadedAddressRules.pop_front();

		// only IP, IP range and coutry rules allowed
		Q_ASSERT( pRule->getType() > 0 && pRule->getType() < 4 );

		delete pRule;
		pRule = NULL;
	}

	while ( m_loadedHitRules.size() )
	{
		pRule = m_loadedHitRules.front();
		m_loadedHitRules.pop_front();

		// Only hit related rules are allowed
		Q_ASSERT( pRule->getType() > 3 );

		delete pRule;
		pRule = NULL;
	}

	m_bNewRulesLoaded = false;

	return true;
}

//////////////////////////////////////////////////////////////////////
// CSecurity ban

void CSecurity::ban(const QHostAddress& oAddress, BanLength nBanLength, bool bMessage, const QString& strComment)
{
	if ( oAddress.isNull() )
	{
//		theApp.Message( MSG_ERROR, IDS_SECURITY_ERROR_IP_BAN );
		return;
	}

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	QMutexLocker mutex( &m_pSection );

	CAddressRuleMap::const_iterator i = m_IPs.find( oAddress.toString() );
	if ( i != m_IPs.end() )
	{
		CSecureRule* pIPRule = (*i).second;

		if ( pIPRule->m_nAction == CSecureRule::srDeny )
		{
			if ( !( strComment.isEmpty() ) )
				pIPRule->m_sComment = strComment;

			quint32 nExpireTime = 0;

			switch( nBanLength )
			{
			case banSession:
				pIPRule->m_tExpire = CSecureRule::srSession;
				return;

			case ban5Mins:
				nExpireTime = tNow + 300;
				break;

			case ban30Mins:
				nExpireTime = tNow + 1800;
				break;

			case ban2Hours:
				nExpireTime = tNow + 7200;
				break;

			case banWeek:
				nExpireTime = tNow + 604800;
				break;

			case banMonth:
				nExpireTime = tNow + 2592000;
				break;

			case banForever:
				pIPRule->m_tExpire = CSecureRule::srIndefinite;
				return;

			default:
				Q_ASSERT( false );
			}

			if ( pIPRule->m_tExpire < nExpireTime )
			{
				pIPRule->m_tExpire = nExpireTime;
			}

			if ( bMessage )
			{
//				theApp.Message( MSG_NOTICE, IDS_NETWORK_SECURITY_ALREADY_BLOCKED,
//					oAddress.toString() );
			}

			return;
		}
		else
		{
			mutex.unlock();
			remove( pIPRule );
			mutex.relock();
		}
	}

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

	if ( !( strComment.isEmpty() ) )
		pIPRule->m_sComment = strComment;

	pIPRule->setIP( oAddress );

	mutex.unlock();
	add( pIPRule );

	if ( bMessage )
	{
//		theApp.Message( MSG_NOTICE, IDS_NETWORK_SECURITY_BLOCKED,
//			(LPCTSTR)FromIP( AF_INET, &oAddress ) );
	}
}

void CSecurity::ban(const CFile& oFile, BanLength nBanLength, bool bMessage, const QString& strComment)
{
	if ( oFile.isNull() )
	{
//		theApp.Message( MSG_ERROR, IDS_SECURITY_ERROR_FILE_BAN, "" );
		return;
	}

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	/*QString sSHA1 = ( pFile->m_oSHA1  ? pFile->m_oSHA1.toUrn()  : "" );
	QString sED2K = ( pFile->m_oED2K  ? pFile->m_oED2K.toUrn()  : "" );
	QString sBTIH = ( pFile->m_oBTH   ? pFile->m_oBTH.toUrn()   : "" );
	QString sTTH  = ( pFile->m_oTiger ? pFile->m_oTiger.toUrn() : "" );
	QString sMD5  = ( pFile->m_oMD5   ? pFile->m_oMD5.toUrn()   : "" );*/

	/*// Lock section before working with iterators...
	QMutexLocker mutex( &m_pSection );

	CIterator i = getHash( pFile->m );

	bool bAlreadyBlocked = ( i != getEnd() && ((CHashRule*)*i)->match( sSHA1, sED2K, sTTH, sBTIH, sMD5 ) );

	if ( bAlreadyBlocked )
	{
		if ( bMessage )
		{
//			theApp.Message( MSG_NOTICE, IDS_SECURITY_ERROR_FILE_ALREADY_BANNED, (LPCTSTR)pFile->m_sName );
		}
	}
	else
	{
		CHashRule* pRule = new CHashRule();

		switch ( nBanLength )
		{
		case banSession:
			pRule->m_nExpire	= CSecureRule::srSession;
			pRule->m_sComment	= "Session Ban";
			break;
		case ban5Mins:
			pRule->m_nExpire	= tNow + 300;
			pRule->m_sComment	= "Temp Ignore";
			break;
		case ban30Mins:
			pRule->m_nExpire	= tNow + 1800;
			pRule->m_sComment	= "Temp Ignore";
			break;
		case ban2Hours:
			pRule->m_nExpire	= tNow + 7200;
			pRule->m_sComment	= "Temp Ignore";
			break;
		case banWeek:
			pRule->m_nExpire	= tNow + 604800;
			pRule->m_sComment	= "Client Block";
			break;
		case banMonth:
			pRule->m_nExpire	= tNow + 2592000; // 60*60*24*30 = 30 days
			pRule->m_sComment	= "Quick IP Block";
			break;
		case banForever:
			pRule->m_nExpire 	= CSecureRule::srIndefinite;
			pRule->m_sComment	= "Ban";
			break;
		default:
			pRule->m_nExpire	= CSecureRule::srSession;
			pRule->m_sComment	= "Session Ban";
			Q_ASSERT( false ); // this should never happen
		}

		if ( !( strComment.isEmpty() ) )
			pRule->m_sComment = strComment;

		// First try to get a pair of important hashes, if that doesn't work, ban everything there is.
		if ( pFile->m_oSHA1 && pFile->m_oED2K ) // Most probable case if G2 hits are present
		{
			pRule->SetContentWords( pFile->m_oSHA1.toUrn()  + " " + pFile->m_oED2K.toUrn() );
		}
		else if ( pFile->m_oSHA1 && pFile->m_oTiger ) // Catches files with gnutella hits
		{
			pRule->SetContentWords( pFile->m_oSHA1.toUrn()  + " " + pFile->m_oTiger.toUrn() );
		}
		else if ( pFile->m_oED2K ) // eD2k network
		{
			pRule->SetContentWords( pFile->m_oED2K.toUrn() );
		}
		// other (these rules will not be really effective as they generally lack the main hashes)
		else if ( pFile->m_oSHA1 || pFile->m_oTiger || pFile->m_oMD5 || pFile->m_oBTH )
		{
			pRule->SetContentWords(
				( pFile->m_oSHA1  ? pFile->m_oSHA1.toUrn()  + " " : QString() ) +
				( pFile->m_oBTH   ? pFile->m_oBTH.toUrn()   + " " : QString() ) +
				( pFile->m_oTiger ? pFile->m_oTiger.toUrn() + " " : QString() ) +
				( pFile->m_oMD5   ? pFile->m_oMD5.toUrn()         : QString() ) );
		}
		else
		{
			// We got no valid hashes from that file.
			QString str = " (File: " + pFile->m_sName + ")";
//			theApp.Message( MSG_ERROR, IDS_SECURITY_ERROR_FILE_BAN, (LPCTSTR)str );
			return;
		}

		mutex.unlock();
		add( pRule );

		if ( bMessage )
		{
//			theApp.Message( MSG_NOTICE, IDS_NETWORK_SECURITY_BLOCKED,
//				(LPCTSTR)pFile->m_sName );
		}
	}*/
}

//////////////////////////////////////////////////////////////////////
// public CSecurity access checks

bool CSecurity::isNewlyDenied(const QHostAddress& oAddress, const QString&)
{
	if ( oAddress.isNull() )
		return false;

	CSecureRule* pRule = NULL;
	QMutexLocker l( &m_pSection );

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

			pRule->count();

			if ( pRule->m_nAction == CSecureRule::srAccept )
			{
				return false;
			}
			else if ( pRule->m_nAction == CSecureRule::srDeny )
			{
				return true;
			}
		}

		i++;
	}

	return false;
}
/*bool CSecurity::isNewlyDenied(const CQueryHit* pHit, const CQuerySearch* pQuery)
{
	if ( !pHit )
		return false;

	CSecureRule* pRule = NULL;
	QMutexLocker l( &m_pSection );

	// This should only be called if new rules have been loaded previously.
	Q_ASSERT( m_bNewRulesLoaded );

	if ( m_loadedHitRules.empty() )
		return false;

	CIterator i = m_loadedHitRules.begin();

	while ( i != m_loadedHitRules.end() )
	{
		pRule = *i;

		if ( pRule->match( (CShareazaFile*)pHit ) || pRule->match( pHit->m_sName ) || pRule->match( pQuery, pHit->m_sName ) )
		{
			// The rules are new, so we don't need to check whether they are expired or not.

			pRule->count();

			if ( pRule->m_nAction == CSecureRule::srAccept )
			{
				return false;
			}
			else if ( pRule->m_nAction == CSecureRule::srDeny )
			{
				return true;
			}
		}

		i++;
	}

	return false;
}*/

bool CSecurity::isDenied(const QHostAddress& oAddress, const QString &source)
{
	if ( oAddress.isNull() )
		return false;

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	QMutexLocker mutex( &m_pSection );

	// Do one of the tasks that have to be done in certain time intervals if necessary...
	// (maximal one of them to reduce lag on function call)
	if ( m_bDelayedSanityCheckRequested && m_nSanityCheckRequestTime + 10 < tNow )
	{
		mutex.unlock();
		sanityCheck();
		mutex.relock();
	}
	else if ( m_nLastRuleExpiryCheck + quazaaSettings.Security.RuleExpiryInterval < tNow )
	{
		expire();
	}
	else if ( m_nLastMissCacheExpiryCheck + quazaaSettings.Security.MissCacheExpiryInterval < tNow )
	{
		mutex.unlock();
		missCacheClear( tNow );
		mutex.relock();
	}

	// First, check the miss cache if the IP is not included in the list of rules.
	// If the address is in cache, it is a miss and no further lookup is needed.
	if ( m_bUseMissCache && m_Cache.count( oAddress.toString() ) )
	{
		if ( quazaaSettings.Security.LogIPCheckHits )
		{
//			theApp.Message( MSG_DEBUG, "Skipped  repeat IP security check for %s (%i IPs cached; Call source: %s)",
//							oAddress.toString(), m_Cache.size(), source );
		}

		return m_bDenyPolicy;
	}
	else if ( quazaaSettings.Security.LogIPCheckHits )
	{
//		theApp.Message( MSG_DEBUG, "Called first-time IP security check for %s (Call source: %s)"),
//						oAddress.toString(), source );
	}

	// Second, check the fast IP rules lookup map.
	CAddressRuleMap::const_iterator i;
	i = m_IPs.find( oAddress.toString() );

	if ( i != m_IPs.end() )
	{
		CSecureRule* pIPRule = (*i).second;
		if ( pIPRule->isExpired( tNow ) )
		{
			mutex.unlock();
			remove( pIPRule );
			mutex.relock();
		}
		else
		{
			pIPRule->count();

			if ( pIPRule->m_tExpire > CSecureRule::srSession && pIPRule->m_tExpire < tNow + 300 )
			{	// Add 5 min penalty for early access
				pIPRule->m_tExpire = tNow + 300;
			}

			if ( pIPRule->m_nAction == CSecureRule::srAccept )
			{
				return false;
			}
			else if ( pIPRule->m_nAction == CSecureRule::srDeny )
			{
				return true;
			}
			else if ( ! ( pIPRule->m_nAction == CSecureRule::srNull ) )
			{	// all rules within m_IPBlocked and m_IPAllowed should either be srAccept or srDeny.
				Q_ASSERT( false );
			}
		}
	}

	// Third, check whether the IP is contained within one of the IP range rules.
	CIPRangeRuleList::iterator it = m_IPRanges.begin();
	while ( it != m_IPRanges.end() )
	{
		CIPRangeRule* pRule = *it;

		if ( pRule->match( oAddress ) )
		{
			if ( pRule->isExpired( tNow ) )
			{
				mutex.unlock();
				remove( pRule ); // invalidates iterator
				mutex.relock();

				it = m_IPRanges.begin();
				continue;
			}

			pRule->count();

			if ( pRule->m_tExpire > CSecureRule::srSession && pRule->m_tExpire < tNow + 300 )
				// Add 5 min penalty for early access
				pRule->m_tExpire = tNow + 300;

			if ( pRule->m_nAction == CSecureRule::srAccept )
			{
				return false;
			}
			else if ( pRule->m_nAction == CSecureRule::srDeny )
			{
				return true;
			}
		}

		it++;
	}

	// If the IP is not within the rules (and we're using the cache),
	// add the IP to the miss cache.
	missCacheAdd( oAddress.toString() );

	// In this case, return our default policy
	return m_bDenyPolicy;
}

/*bool CSecurity::isDenied(const CQueryHit* pHit, const CQuerySearch* pQuery)
{
	return ( isDenied( (CShareazaFile*)pHit ) || isDenied( pHit->m_sName ) || isDenied( pQuery, pHit->m_sName ) );
}*/

// If the remote computer is running a client that is breaking GPL, causing problems, etc.
// We don't actually ban these clients, but we don't accept them as a leaf. Can still upload, though.
bool CSecurity::isClientBad(const QString& sUserAgent) const
{
	// No user agent- assume bad
	if ( sUserAgent.isEmpty() )					return true; // They allowed to connect but no searches were performed

	QString sSubStr;

	// Bad/unapproved versions of Shareaza
	// Really obsolete versions of Shareaza should be blocked. (they may have bad settings)
	if ( sUserAgent.startsWith( "shareaza" ) )
	{
		sSubStr = sUserAgent.mid( 8 );
		if ( sSubStr.startsWith( " 0."  ) )			return true;
		if ( sSubStr.startsWith( " 1."  ) )			return true;	// There can be some 1.x versions of the real Shareaza but most are fakes
		if ( sSubStr.startsWith( " 2.0" ) )			return true;	// There is also a Shareaza rip-off that identify as Shareaza 2.0.0.0 (The real Shareaza 2.0.0.0 is so old and bad)
		if ( sSubStr.startsWith( " 3.0" ) )			return true;
		if ( sSubStr.startsWith( " 3.1" ) )			return true;
		if ( sSubStr.startsWith( " 3.2" ) )			return true;
		if ( sSubStr.startsWith( " 3.3" ) )			return true;
		if ( sSubStr.startsWith( " 3.4" ) )			return true;
		if ( sSubStr.startsWith( " 6."  ) )			return true;
		if ( sSubStr.startsWith( " 7."  ) )			return true;
		if ( sSubStr.startsWith( " Pro" ) )			return true;

		return false;
	}

	// Dianlei: Shareaza rip-off
	// add only based on alpha code, need verification for others
	if ( sUserAgent.startsWith( "Dianlei" ) )
	{
		sSubStr = sUserAgent.mid( 7 );
		if ( sSubStr.startsWith( " 1." ) )			return true;
		if ( sSubStr.startsWith( " 0." ) )			return true;

		return false;
	}

	// BearShare
	if ( sUserAgent.startsWith( "BearShare" ) )
	{
		sSubStr = sUserAgent.mid( 9 );
		if ( sSubStr.startsWith( " Lite"  ) )		return true;
		if ( sSubStr.startsWith( " Pro"   ) )		return true;
		if ( sSubStr.startsWith( " MP3"   ) ) 		return true;	// GPL breaker
		if ( sSubStr.startsWith( " Music" ) ) 		return true;	// GPL breaker
		if ( sSubStr.startsWith( " 6."    ) ) 		return true;	// iMesh

		return false;
	}

	// Fastload.TV
	if ( sUserAgent.startsWith( "Fastload.TV" ) )				return true;

	// Fildelarprogram
	if ( sUserAgent.startsWith( "Fildelarprogram" ) )			return true;

	// Gnutella Turbo (Look into this client some more)
	if ( sUserAgent.startsWith( "Gnutella Turbo" ) )			return true;

	// Identified Shareaza Leecher Mod
	if ( sUserAgent.startsWith( "eMule mod (4)" ) )				return true;

	// iMesh
	if ( sUserAgent.startsWith( "iMesh" ) )						return true;

	// Mastermax File Sharing
	if ( sUserAgent.startsWith( "Mastermax File Sharing" ) )	return true;

	// Trilix
	if ( sUserAgent.startsWith( "Trilix" ) )					return true;

	// Wru
	if ( sUserAgent.startsWith( "Wru" ) )						return true; // bad GuncDNA based client

	// GPL breakers- Clients violating the GPL
	// See http://www.gnu.org/copyleft/gpl.html
	// Some other breakers outside the list

	if ( sUserAgent.startsWith( "C -3.0.1" ) )					return true;

	if ( sUserAgent.startsWith( "eTomi" ) )						return true; // outdated rip-off

	if ( sUserAgent.startsWith( "FreeTorrentViewer" ) )			return true; // Shareaza rip-off / GPL violator

	if ( sUserAgent.startsWith( "K-Lite" ) )					return true; // Is it bad?

	if ( sUserAgent.startsWith( "mxie" ) )						return true; // Leechers, do not allow to connect

	if ( sUserAgent.startsWith( "P2P Rocket" ) )				return true; // Shareaza rip-off / GPL violator

	if ( sUserAgent.startsWith( "SlingerX" ) )					return true; // Rip-off with bad tweaks

	if ( sUserAgent.startsWith( "vagaa" ) )						return true; // Not clear why it's bad

	if ( sUserAgent.startsWith( "WinMX" ) )						return true;

	// Unknown- Assume OK
	return false;
}

// This for especially bad / leecher clients, as well as user defined agent blocks.
bool CSecurity::isAgentBlocked(const QString& sUserAgent)
{
	// The remote computer didn't send a "User-Agent", or it sent whitespace
	// We don't like those.
	if ( sUserAgent.isEmpty() )									return true;

	// i2hub - leecher client. (Tested, does not upload)
	if ( sUserAgent.startsWith( "i2hub 2.0" ) )					return true;

	// foxy - leecher client. (Tested, does not upload)
	// having something like Authentication which is not defined on specification
	if ( sUserAgent.startsWith( "foxy" ) )						return true;

	// Check by content filter
	return isAgentDenied( sUserAgent );
}

bool CSecurity::isVendorBlocked(const QString& sVendor) const
{
	// foxy - leecher client. (Tested, does not upload)
	// having something like Authentication which is not defined on specification
	if ( sVendor.startsWith( "foxy" ) )						return true;

	// Allow it
	return false;
}

//////////////////////////////////////////////////////////////////////
// CSecurity load and save

bool CSecurity::load()
{
	QString sPath = quazaaSettings.Security.DataPath.append( "security.dat" );

	if ( load( sPath ) )
	{
		return true;
	}
	else
	{
		sPath = quazaaSettings.Security.DataPath.append( "security_backup.dat" );
		return load( sPath );
	}
}

bool CSecurity::load( QString sPath )
{
	QFile oFile( sPath );

	if ( ! oFile.open( QIODevice::ReadOnly ) )
		return false;

	CSecureRule* pRule = NULL;

	try
	{
		clear();

		QDataStream oStream( &oFile );

		int nVersion;
		oStream >> nVersion;

		bool bDenyPolicy;
		oStream >> bDenyPolicy;

		unsigned int nCount;
		oStream >> nCount;

		quint32 tNow = static_cast< quint32 >( time( NULL ) );

		QMutexLocker mutex( &m_pSection );
		m_bDenyPolicy = bDenyPolicy;
		m_bIsLoading = true;
		mutex.unlock();

		while ( nCount > 0 )
		{
			CSecureRule::load( pRule, oStream, nVersion );

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

		QMutexLocker l( &m_pSection );
		m_bIsLoading = false;

		return false;
	}
	oFile.close();

	return true;
}

bool CSecurity::save()
{
	QMutexLocker mutex( &m_pSection );

	if ( m_bSaved )		// Saving not required ATM.
	{
		return true;
	}
	mutex.unlock();

	QString sBackupPath = quazaaSettings.Security.DataPath.append( "security_backup.dat" );
	QString sPath		= quazaaSettings.Security.DataPath.append( "security.dat" );

	if ( QFile::exists( sBackupPath ) )
	{
		if ( !QFile::remove( sBackupPath ) )
			return false; // Error while removing old backup file.
	}

	if ( QFile::exists( sPath ) )
	{
		if ( !QFile::rename( sPath, sBackupPath ) )
			return false; // Error while renaming current database file to replace backup file.
	}

	QFile oFile( sPath );

	if ( ! oFile.open( QIODevice::WriteOnly ) )
		return false;

	int nVersion = SECURITY_CODE_VERSION;

	try
	{
		QDataStream oStream( &oFile );

		oStream << nVersion;

		// Security manager must be kept locked while working with iterators.
		mutex.relock();
		oStream << m_bDenyPolicy;

		oStream << getCount();

		for ( CIterator i = getIterator(); i != getEnd(); i++ )
		{
			const CSecureRule* pRule = *i;
			CSecureRule::save( pRule, oStream );
		}
		mutex.unlock();
	}
	catch ( ... )
	{

		return false;
	}

	mutex.relock();
	m_bSaved = true;
	mutex.unlock();

	// We don't really care whether this fails or not, as saving to the primary file was successfull.
	QFile::remove( sBackupPath );
	QFile::copy( sPath, sBackupPath );

	return true;
}

//////////////////////////////////////////////////////////////////////
// CSecurity import

bool CSecurity::import(const QString &sPath)
{
	QFile oFile( sPath );

	if ( !oFile.open( QIODevice::ReadOnly ) )
		return false;

	QDomDocument oXMLroot( "security" );

	if ( oXMLroot.setContent( &oFile ) )
	{
		bool bResult = fromXML( oXMLroot );
		sanityCheck();
		return bResult;
	}
	else
	{
		oFile.close();
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
// CSecurity XML

const QString CSecurity::xmlns = "http://www.shareaza.com/schemas/Security.xsd";

bool CSecurity::fromXML(const QDomDocument &oXMLroot)
{
	if ( oXMLroot.nodeName() != "security" )
		return false;

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	QDomNodeList oXMLnodes = oXMLroot.childNodes();

	const unsigned int nCount = oXMLnodes.length();

	QDomElement oXMLelement;
	CSecureRule* pRule = NULL;
	unsigned int nRuleCount = 0;

	QMutexLocker mutex( &m_pSection );
	m_bIsLoading = true;
	mutex.unlock();

	for ( unsigned int i = 0; i < nCount; i++ )
	{
		if ( oXMLnodes.item( i ).isElement() )
		{
			oXMLelement = oXMLnodes.item( i ).toElement();
		}
		else
		{
			continue;
		}

		if ( oXMLelement.nodeName() == "rule" )
		{
			pRule = CSecureRule::fromXML( oXMLelement );

			if ( pRule )
			{
				if ( getUUID( pRule->m_oUUID ) == getEnd() && !pRule->isExpired( tNow ) )
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
//				theApp.Message( MSG_ERROR, IDS_SECURITY_XML_RULE_IMPORT_FAIL );
			}
		}
	}

	mutex.relock();
	m_bIsLoading = false;

	return nCount > 0;
}

QDomDocument CSecurity::toXML()
{
	QDomDocument oXMLdoc = QDomDocument( "security" );
	QDomElement oXMLroot = oXMLdoc.createElement( "security" );
	oXMLroot.setAttribute( "xmlns", CSecurity::xmlns );

	for ( CIterator i = getIterator(); i != getEnd() ; i++ )
	{
		// Invalid rules could return NULL.
		(*i)->toXML( oXMLroot );
	}

	return oXMLdoc;
}

//////////////////////////////////////////////////////////////////////
// Private method definitions

CSecurity::CIterator CSecurity::getHash(const QString& sSHA1,
										const QString& sED2K,
										const QString& sTTH ,
										const QString& sBTIH,
										const QString& sMD5  ) const
{
	bool bSHA1 = !( sSHA1.isEmpty() );
	bool bED2K = !( sED2K.isEmpty() );
	bool bTTH  = !(  sTTH.isEmpty() );
	bool bBTIH = !( sBTIH.isEmpty() );
	bool bMD5  = !(  sMD5.isEmpty() );

	// Find out how many hashes we are searching for.
	quint8 count = bSHA1 + bED2K + bBTIH + bTTH + bMD5;

	// We are not searching for any hash. :)
	if ( !count ) return getEnd();

	CHashRuleMap::const_iterator i;

	i = m_Hashes.find( sSHA1 );

	while ( i != m_Hashes.end() )
	{
		if ( (*i).second->match(sSHA1, sED2K, sTTH, sBTIH, sMD5) ) return getUUID( (*i).second->m_oUUID );
		i++;

		if( (*i).first != sSHA1 ) break;
	}

	i = m_Hashes.find( sED2K );

	while ( i != m_Hashes.end() )
	{
		if ( (*i).second->match(sSHA1, sED2K, sTTH, sBTIH, sMD5) ) return getUUID( (*i).second->m_oUUID );
		i++;

		if ( (*i).first != sED2K ) break;
	}

	i = m_Hashes.find( sBTIH );

	while ( i != m_Hashes.end() )
	{
		if ( (*i).second->match(sSHA1, sED2K, sTTH, sBTIH, sMD5) ) return getUUID( (*i).second->m_oUUID );
		i++;

		if ( (*i).first != sBTIH ) break;
	}

	i = m_Hashes.find( sTTH );

	while ( i != m_Hashes.end() )
	{
		if ( (*i).second->match(sSHA1, sED2K, sTTH, sBTIH, sMD5) ) return getUUID( (*i).second->m_oUUID );
		i++;

		if ( (*i).first != sTTH ) break;
	}

	i = m_Hashes.find( sMD5 );

	while ( i != m_Hashes.end() )
	{
		if ( (*i).second->match(sSHA1, sED2K, sTTH, sBTIH, sMD5) ) return getUUID( (*i).second->m_oUUID );
		i++;

		if ( (*i).first != sMD5 ) break;
	}

	return getEnd();
}
CSecurity::CIterator CSecurity::getUUID(const QUuid& oUUID) const
{
	for ( CIterator i = getIterator() ; i != getEnd(); i++ )
	{
		if ( (*i)->m_oUUID == oUUID ) return i;
	}

	return getEnd();
}

CSecurity::CIterator CSecurity::remove(CIterator it)
{
	if ( it == getEnd() )
		return it;

	CSecureRule* pRule = *it;
	CSecureRule::RuleType type = pRule->getType();

	// Removing the rule from special containers for fast access.
	switch ( type )
	{
	case CSecureRule::srContentAddress:
		{
			QString sIP = ((CIPRule*)pRule)->getIP().toString();
			CAddressRuleMap::iterator i = m_IPs.find( sIP );

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

				i++;
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

			QString sSHA1 = pHashRule->getSHA1urn();
			QString sED2K = pHashRule->getED2Kurn();
			QString sBTIH = pHashRule->getBTIHurn();
			QString sTTH  = pHashRule->getTTHurn();
			QString sMD5  = pHashRule->getMD5urn();

			CHashRuleMap::iterator i;

			i = m_Hashes.find( sSHA1 );

			while ( i != m_Hashes.end() && (*i).second->getSHA1urn() == sSHA1 )
			{
				if ( (*i).second->m_oUUID == pHashRule->m_oUUID )
				{
					m_Hashes.erase( i++ );
				}
				else
				{
					i++;
				}
			}

			i = m_Hashes.find( sED2K );

			while ( i != m_Hashes.end() && (*i).second->getED2Kurn() == sED2K )
			{
				if ( (*i).second->m_oUUID == pHashRule->m_oUUID )
				{
					m_Hashes.erase( i++ );
				}
				else
				{
					i++;
				}
			}

			i = m_Hashes.find( sBTIH );

			while ( i != m_Hashes.end() && (*i).second->getBTIHurn() == sBTIH )
			{
				if ( (*i).second->m_oUUID == pHashRule->m_oUUID )
				{
					m_Hashes.erase( i++ );
				}
				else
				{
					i++;
				}
			}

			i = m_Hashes.find( sTTH );

			while ( i != m_Hashes.end() && (*i).second->getTTHurn() == sTTH )
			{
				if ( (*i).second->m_oUUID == pHashRule->m_oUUID )
				{
					m_Hashes.erase( i++ );
				}
				else
				{
					i++;
				}
			}

			i = m_Hashes.find( sMD5 );

			while ( i != m_Hashes.end() && (*i).second->getMD5urn() == sMD5 )
			{
				if ( (*i).second->m_oUUID == pHashRule->m_oUUID )
				{
					m_Hashes.erase( i++ );
				}
				else
				{
					i++;
				}
			}
		}
		break;

	case CSecureRule::srContentAll:
	case CSecureRule::srContentAny:
		{
			CContentRuleList::iterator i = m_Contents.begin();

			while ( i != m_Contents.end() )
			{
				if ( (*i)->m_oUUID == pRule->m_oUUID )
				{
					m_Contents.erase( i );
					break;
				}

				i++;
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

				i++;
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

				i++;
			}
		}
		break;

	default:
		Q_ASSERT( false );
	}

	// The only reciever, CSecurityTableModel, deals with releasing the memory.
	emit ruleRemoved( pRule );
//	delete pRule;

	m_bSaved = false;

	// Remove rule entry from list of all rules
	return m_Rules.begin();//.erase( it );
}

bool CSecurity::isAgentDenied(const QString& strUserAgent)
{
	if ( strUserAgent.isEmpty() )
		return false;

	// Private member function. Only called from places that have already a lock.

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	CUserAgentRuleMap::iterator i = m_UserAgents.find( strUserAgent );
	if ( i != m_UserAgents.end() )
	{
		CUserAgentRule* pRule = (*i).second;

		if ( pRule->match( strUserAgent ) )
		{
			if ( pRule->isExpired( tNow ) )
			{
				remove( pRule );
			}

			pRule->count();

			if ( pRule->m_tExpire > CSecureRule::srSession &&
				pRule->m_tExpire < tNow + 300 )
				// Add 5 min penalty for early access
				pRule->m_tExpire = tNow + 300;

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}
	}

	return false;
}

void CSecurity::missCacheAdd( const QString &sIP )
{
	if ( m_bUseMissCache )
	{
		m_Cache.insert( sIP );
		evaluateCacheUsage();
	}
}
void CSecurity::missCacheClear( const quint32 &tNow )
{
	m_Cache.clear();
	m_nLastMissCacheExpiryCheck = tNow;

	if ( !m_bUseMissCache )
		evaluateCacheUsage();
}
void CSecurity::evaluateCacheUsage()
{
	double nCache		= m_Cache.size();
	double nIPMap		= m_IPs.size();
	double nCountryMap	= m_Countries.size();

	if ( ! m_Cache.size() )		nCache = 1;
	if ( ! m_IPs.size() )		nIPMap = 1;
	if ( ! m_Countries.size() )	nCountryMap = 1;

	m_bUseMissCache = ( log( nCache ) < ( log( nIPMap * nCountryMap ) + m_IPRanges.size() * log( 2.0 ) ) );
}

bool CSecurity::isDenied(const QString& strContent)
{
	if ( strContent.isEmpty() )
		return false;

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	QMutexLocker mutex( &m_pSection );

	CContentRuleList::iterator i = m_Contents.begin();
	while ( i != m_Contents.end() )
	{
		CContentRule* pRule = *i;

		if ( pRule->match( strContent ) )
		{
			if ( pRule->isExpired( tNow ) )
			{
				mutex.unlock();
				remove( pRule );
				mutex.relock();

				i = m_Contents.begin();
				continue;
			}

			pRule->count();

			if ( pRule->m_tExpire > CSecureRule::srSession &&
				pRule->m_tExpire < tNow + 300 )
				// Add 5 min penalty for early access
				pRule->m_tExpire = tNow + 300;

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}

		i++;
	}

	return false;
}
/*bool CSecurity::isDenied(const CShareazaFile* pFile)
{
	if ( !pFile )
		return false;

	// First get all available hashes.
	QString sSHA1 = ( pFile->m_oSHA1  ? pFile->m_oSHA1.toUrn()  : "" );
	QString sED2K = ( pFile->m_oED2K  ? pFile->m_oED2K.toUrn()  : "" );
	QString sBTIH = ( pFile->m_oBTH   ? pFile->m_oBTH.toUrn()   : "" );
	QString sTTH  = ( pFile->m_oTiger ? pFile->m_oTiger.toUrn() : "" );
	QString sMD5  = ( pFile->m_oMD5   ? pFile->m_oMD5.toUrn()   : "" );

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	QMutexLocker mutex( &m_pSection );

	// Search for a rule matching these hashes
	CIterator it = getHash( sSHA1, sED2K, sTTH, sBTIH, sMD5 );

	// If this rule matches the file, return denied.
	if ( it != getEnd() && ((CHashRule*)*it)->match( sSHA1, sED2K, sTTH, sBTIH, sMD5 ) )
		return true;

	// Else check other content rules.
	CContentRuleList::iterator i = m_Contents.begin();
	while ( i != m_Contents.end() )
	{
		CContentRule* pRule = *i;

		if ( pRule->match( pFile ) )
		{
			if ( pRule->isExpired( tNow ) )
			{
				mutex.unlock();
				remove( pRule );
				mutex.relock();

				i = m_Contents.begin();
				continue;
			}

			pRule->count();

			if ( pRule->m_nExpire > CSecureRule::srSession &&
				pRule->m_nExpire < tNow + 300 )
				// Add 5 min penalty for early access
				pRule->m_nExpire = tNow + 300;

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}

		i++;
	}

	return false;
}*/
/*bool CSecurity::isDenied(const CQuerySearch* pQuery, const QString& strContent)
{
	if ( !pQuery || strContent.isEmpty() )
		return false;

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	QMutexLocker mutex( &m_pSection );

	CRegExpRuleList::iterator i = m_RegExpressions.begin();
	while ( i != m_RegExpressions.end() )
	{
		CRegExpRule* pRule = *i;

		if ( pRule->match( pQuery, strContent ) )
		{
			if ( pRule->isExpired( tNow ) )
			{
				mutex.unlock();
				remove( pRule );
				mutex.relock();

				i = m_RegExpressions.begin();

				continue;
			}

			pRule->count();

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}

		i++;
	}

	return false;
}*/

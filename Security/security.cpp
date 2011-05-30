#include <math.h>

#include "security.h"

using namespace security;

CSecurity Security;

CSecurity::CSecurity()
{
	m_bDenyPolicy = false;
	m_bIsLoading  = false;
	m_bNewRulesLoaded = false;
	m_bDelayedSanityCheckRequested = false;

	// We can't load from settings here as they haven't been loaded when this is initialized.
	m_bDebug = true;
	m_nRuleExpiryInterval = 0;
	m_nMissCacheExpiryInterval = 0;
	m_nSettingsLoadInterval = 60; // load variables from settings each 60 seconds

	m_nLastRuleExpiryCheck = static_cast< quint32 >( time( NULL ) );
	m_nLastMissCacheExpiryCheck	= m_nLastRuleExpiryCheck;
	m_nLastSettingsLoadTime		= m_nLastRuleExpiryCheck;

	m_bSaved = true;
	m_bUseMissCache = false;
}

CSecurity::~CSecurity()
{
	Clear();
}

void CSecurity::DenyPolicy(bool bDenyPolicy)
{
	QMutexLocker l( &m_pSection );

	if( m_bDenyPolicy != bDenyPolicy )
	{
		m_bDenyPolicy = bDenyPolicy;
	}
}

bool CSecurity::Check(CSecureRule* pRule) const
{
	QMutexLocker l( &m_pSection );

	return pRule != NULL && GetUUID( pRule->m_oUUID ) != GetEnd();
}

//////////////////////////////////////////////////////////////////////
// CSecurity rule modification
void CSecurity::Add(CSecureRule* pRule)
{
	if ( !pRule ) return;

	// check for invalid rules
	Q_ASSERT( pRule->Type() > 0 && pRule->Type() < 9 && pRule->m_nAction < 3 );

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	CSecureRule::RuleType type = pRule->Type();
	CSecureRule* pExRule = NULL;

	bool bNewAddress = false;
	bool bNewHit	 = false;

	QMutexLocker mutex( &m_pSection );

	// Special treatment for the different types of rules
	switch ( type )
	{
	case CSecureRule::srContentAddress:
		{
			QString sIP = ((CIPRule*)pRule)->m_oIP.toString();
			CAddressRuleMap::iterator i = m_IPs.find( sIP );

			if ( i != m_IPs.end() ) // there is a potentially conflicting rule in our map
			{
				pExRule = (*i).second;
				if ( pExRule->m_oUUID   != pRule->m_oUUID   ||
					 pExRule->m_nAction != pRule->m_nAction ||
					 pExRule->m_nExpire != pRule->m_nExpire )
				{
					// remove conflicting rule if one of the important attributes
					// differs from the rule we'd like to add
					mutex.unlock();
					Remove( pExRule );
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
				EvaluateCacheUsage();
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
						 pOldRule->m_nExpire != pRule->m_nExpire ||
						 pOldRule->m_oIP     != ((CIPRangeRule*)pRule)->m_oIP ||
						 pOldRule->m_nMask   != ((CIPRangeRule*)pRule)->m_nMask )
					{
						// remove conflicting rule if one of the important attributes
						// differs from the rule we'd like to add
						mutex.unlock();
						Remove( pOldRule );
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
				EvaluateCacheUsage();
		}
		break;

	case CSecureRule::srContentCountry:
		{
			QString country = ((CCountryRule*)pRule)->GetContent();
			CCountryRuleMap::iterator i = m_Countries.find( country );

			bNewAddress = true;

			if ( i != m_Countries.end() ) // there is a potentially conflicting rule in our map
			{
				pExRule = (*i).second;
				if ( pExRule->m_oUUID   != pRule->m_oUUID   ||
					 pExRule->m_nAction != pRule->m_nAction ||
					 pExRule->m_nExpire != pRule->m_nExpire )
				{
					// remove conflicting rule if one of the important attributes
					// differs from the rule we'd like to add
					mutex.unlock();
					Remove( pExRule );
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
				EvaluateCacheUsage();
		}
		break;

	case CSecureRule::srContentHash:
		{
			CHashRule* pHashRule = (CHashRule*)pRule;

			QString sSHA1 = pHashRule->GetSHA1urn();
			QString sED2K = pHashRule->GetED2Kurn();
			QString sBTIH = pHashRule->GetBTIHurn();
			QString sTTH  = pHashRule->GetTTHurn();
			QString sMD5  = pHashRule->GetMD5urn();

			CIterator i = GetHash( sSHA1, sED2K, sTTH, sBTIH, sMD5 );

			if ( i != GetEnd() )
			{
				pExRule = *i;
				if ( ((CHashRule*)*i)->GetHashWeight() == pHashRule->GetHashWeight() )
				{
					if ( pExRule->m_oUUID   != pRule->m_oUUID   ||
						 pExRule->m_nAction != pRule->m_nAction ||
						 pExRule->m_nExpire != pRule->m_nExpire )
					{
						// remove conflicting rule if one of the important attributes
						// differs from the rule we'd like to add
						mutex.unlock();
						Remove( pExRule );
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
						 pOldRule->m_nExpire != pRule->m_nExpire ||
						 pOldRule->GetContentWords() != ((CRegExpRule*)pRule)->GetContentWords() )
					{
						// remove conflicting rule if one of the important attributes
						// differs from the rule we'd like to add
						mutex.unlock();
						Remove( pOldRule );
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
						 pOldRule->m_nExpire != pRule->m_nExpire ||
						 pOldRule->GetContentWords() != ((CRegExpRule*)pRule)->GetContentWords() )
					{
						// remove conflicting rule if one of the important attributes
						// differs from the rule we'd like to add
						mutex.unlock();
						Remove( pOldRule );
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
			QString agent = ((CUserAgentRule*)pRule)->GetContentWords();
			CUserAgentRuleMap::iterator i = m_UserAgents.find( agent );

			if ( i != m_UserAgents.end() ) // there is a conflicting rule in our map
			{
				pExRule = (*i).second;
				if ( pExRule->m_oUUID   != pRule->m_oUUID   ||
					 pExRule->m_nAction != pRule->m_nAction ||
					 pExRule->m_nExpire != pRule->m_nExpire )
				{
					// remove conflicting rule if one of the important attributes
					// differs from the rule we'd like to add
					mutex.unlock();
					Remove( pExRule );
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
		m_Cache.erase( ((CIPRule*)pRule)->m_oIP.toString() );

		if ( !m_bUseMissCache )
			EvaluateCacheUsage();
	}
	else if ( type == CSecureRule::srContentAddressRange || type == CSecureRule::srContentCountry )
	{
		MissCacheClear( tNow );

		if ( !m_bUseMissCache )
			EvaluateCacheUsage();
	}

	// Add rule to list of all rules
	CIterator iExRule = GetUUID( pRule->m_oUUID );
	if ( iExRule != GetEnd() ) // we do not allow 2 rules by the same UUID
	{
		Remove( iExRule );
	}
	m_Rules.push_front( pRule );

	if ( bNewAddress )	// only add IP, IP range and country rules to the queue
		m_newAddressRules.push( pRule->GetCopy() );

	if ( bNewHit )		// only add rules related to hit filtering to the queue
		m_newHitRules.push( pRule->GetCopy() );

	m_bSaved = false;

	// Check all lists for newly denied hosts
	if ( !m_bIsLoading )
	{
		// Unlock mutex before performing system wide security check.
		mutex.unlock();
		// In case we are currently loading rules from file,
		// this is done uppon completion of the entire process.
		SanityCheck();
	}
}

void CSecurity::Remove(CSecureRule* pRule)
{
	if ( !pRule ) return;

	QMutexLocker l( &m_pSection );

	CIterator i = GetUUID( pRule->m_oUUID );
	Remove( i );
}

void CSecurity::Clear()
{
	QMutexLocker mutex( &m_pSection );

	m_IPs.clear();

	m_IPRanges.clear();

	m_Hashes.clear();
	m_RegExpressions.clear();
	m_Contents.clear();
	m_UserAgents.clear();

	CSecureRule* pRule = NULL;
	for ( CIterator i = GetIterator(); i != GetEnd(); i++ )
	{
		pRule = *i;
		delete pRule;
	}
	m_Rules.clear();

	for ( CIterator i = m_loadedAddressRules.begin(); i != m_loadedAddressRules.end(); i++ )
	{
		pRule = *i;
		delete pRule;
	}
	m_loadedAddressRules.clear();

	for ( CIterator i = m_loadedHitRules.begin(); i != m_loadedHitRules.end(); i++ )
	{
		pRule = *i;
		delete pRule;
	}
	m_loadedHitRules.clear();

	while ( m_newAddressRules.size() )
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
	m_Rules.clear();

	quint32 tNow = static_cast< quint32 >( time( NULL ) );
	m_nLastRuleExpiryCheck = tNow;

	MissCacheClear( tNow );

	m_bSaved = false;
}

void CSecurity::Expire()
{
	QMutexLocker mutex( &m_pSection );

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	CIterator i = GetIterator();
	while (  i != GetEnd() )
	{
		if ( (*i)->IsExpired( tNow ) )
		{
			i = Remove( i );
		}
		else
		{
			i++;
		}
	}

	m_nLastRuleExpiryCheck = tNow;
}

void CSecurity::SanityCheck(bool bDelay)
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
		bool bTmp = true; // = !PostMainWndMessage( WM_SANITY_CHECK );

		// Relock mutex again before changing member variables.
		mutex.relock();
		m_bDelayedSanityCheckRequested = bTmp;
	}

	if ( m_bDelayedSanityCheckRequested )
		m_nSanityCheckRequestTime = static_cast< quint32 >( time( NULL ) );
}

bool CSecurity::LoadNewRules()
{
	// If we cannot aquire a lock, return this...
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
		Q_ASSERT( pRule->Type() != 0 && pRule->Type() < 4 );

		m_loadedAddressRules.push_back( pRule );

		pRule = NULL;
	}

	while ( m_newHitRules.size() )
	{
		pRule = m_newHitRules.front();
		m_newHitRules.pop();

		// Only hit related rules are allowed.
		Q_ASSERT( pRule->Type() > 3 );

		m_loadedHitRules.push_back( pRule );

		pRule = NULL;
	}

	m_bNewRulesLoaded = true;

	// Unlocking is necessary as we're not using QMutexLocker.
	m_pSection.unlock();

	return true;
}

bool CSecurity::ClearNewRules()
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
		Q_ASSERT( pRule->Type() > 0 && pRule->Type() < 4 );

		delete pRule;
		pRule = NULL;
	}

	while ( m_loadedHitRules.size() )
	{
		pRule = m_loadedHitRules.front();
		m_loadedHitRules.pop_front();

		// Only hit related rules are allowed
		Q_ASSERT( pRule->Type() > 3 );

		delete pRule;
		pRule = NULL;
	}

	m_bNewRulesLoaded = false;

	return true;
}

//////////////////////////////////////////////////////////////////////
// CSecurity ban

/*void CSecurity::Ban(const CShareazaFile* pFile, BanLength nBanLength, bool bMessage, const QString& strComment)
{
	if ( !pFile )
	{
		theApp.Message( MSG_ERROR, IDS_SECURITY_ERROR_FILE_BAN, "" );
		return;
	}

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	QString sSHA1 = ( pFile->m_oSHA1  ? pFile->m_oSHA1.toUrn()  : "" );
	QString sED2K = ( pFile->m_oED2K  ? pFile->m_oED2K.toUrn()  : "" );
	QString sBTIH = ( pFile->m_oBTH   ? pFile->m_oBTH.toUrn()   : "" );
	QString sTTH  = ( pFile->m_oTiger ? pFile->m_oTiger.toUrn() : "" );
	QString sMD5  = ( pFile->m_oMD5   ? pFile->m_oMD5.toUrn()   : "" );

	// Lock section before working with iterators...
	QMutexLocker mutex( &m_pSection );

	CIterator i = GetHash( sSHA1, sED2K, sTTH, sBTIH, sMD5 );

	bool bAlreadyBlocked = ( i != GetEnd() && ((CHashRule*)*i)->Match( sSHA1, sED2K, sTTH, sBTIH, sMD5 ) );

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
		Add( pRule );

		if ( bMessage )
		{
//			theApp.Message( MSG_NOTICE, IDS_NETWORK_SECURITY_BLOCKED,
//				(LPCTSTR)pFile->m_sName );
		}
	}
}*/

void CSecurity::Ban(const QHostAddress* pAddress, BanLength nBanLength, bool bMessage, const QString& strComment)
{
	if ( !pAddress )
	{
//		theApp.Message( MSG_ERROR, IDS_SECURITY_ERROR_IP_BAN );
		return;
	}

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	QMutexLocker mutex( &m_pSection );

	CAddressRuleMap::const_iterator i = m_IPs.find( pAddress->toString() );
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
				pIPRule->m_nExpire = CSecureRule::srSession;
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
				pIPRule->m_nExpire = CSecureRule::srIndefinite;
				return;

			default:
				Q_ASSERT( false );
			}

			if ( pIPRule->m_nExpire < nExpireTime )
			{
				pIPRule->m_nExpire = nExpireTime;
			}

			if ( bMessage )
			{
				QHostAddress tmpAddr = *pAddress;
//				theApp.Message( MSG_NOTICE, IDS_NETWORK_SECURITY_ALREADY_BLOCKED,
//					tmpAddr.toString() );
			}

			return;
		}
		else
		{
			mutex.unlock();
			Remove( pIPRule );
			mutex.relock();
		}
	}

	CIPRule* pIPRule = new CIPRule();

	switch( nBanLength )
	{
	case banSession:
		pIPRule->m_nExpire	= CSecureRule::srSession;
		pIPRule->m_sComment	= "Session Ban";
		break;
	case ban5Mins:
		pIPRule->m_nExpire	= tNow + 300;
		pIPRule->m_sComment	= "Temp Ignore";
		break;
	case ban30Mins:
		pIPRule->m_nExpire	= tNow + 1800;
		pIPRule->m_sComment	= "Temp Ignore";
		break;
	case ban2Hours:
		pIPRule->m_nExpire	= tNow + 7200;
		pIPRule->m_sComment	= "Temp Ignore";
		break;
	case banWeek:
		pIPRule->m_nExpire	= tNow + 604800;
		pIPRule->m_sComment	= "Client Block";
		break;
	case banMonth:
		pIPRule->m_nExpire	= tNow + 2592000; // 60*60*24*30 = 30 days
		pIPRule->m_sComment	= "Quick IP Block";
		break;
	case banForever:
		pIPRule->m_nExpire	= CSecureRule::srIndefinite;
		pIPRule->m_sComment	= "Ban";
		break;
	default:
		pIPRule->m_nExpire	= CSecureRule::srSession;
		pIPRule->m_sComment	= "Session Ban";
		Q_ASSERT( false ); // this should never happen
	}

	if ( !( strComment.isEmpty() ) )
		pIPRule->m_sComment = strComment;

	pIPRule->m_oIP = *pAddress;

	mutex.unlock();
	Add( pIPRule );

	if ( bMessage )
	{
		QHostAddress tmpAddr = *pAddress;
//		theApp.Message( MSG_NOTICE, IDS_NETWORK_SECURITY_BLOCKED,
//			(LPCTSTR)FromIP( AF_INET, &tmpAddr ) );
	}
}

//////////////////////////////////////////////////////////////////////
// public CSecurity access checks

bool CSecurity::IsNewlyDenied(const QHostAddress* pAddress, const QString&)
{
	if ( !pAddress )
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

		if ( pRule->Match( pAddress ) )
		{
			// the rules are new, so we don't need to check whether they are expired or not

			pRule->Count();

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
/*bool CSecurity::IsNewlyDenied(const CQueryHit* pHit, const CQuerySearch* pQuery)
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

		if ( pRule->Match( (CShareazaFile*)pHit ) || pRule->Match( pHit->m_sName ) || pRule->Match( pQuery, pHit->m_sName ) )
		{
			// The rules are new, so we don't need to check whether they are expired or not.

			pRule->Count();

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

bool CSecurity::IsDenied(const QHostAddress* pAddress, const QString &source)
{
	if ( !pAddress )
		return false;

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	QMutexLocker mutex( &m_pSection );

	// Do one of the tasks that have to be done in certain time intervals if necessary...
	// (maximal one of them to reduce lag on function call)
	if ( m_bDelayedSanityCheckRequested && m_nSanityCheckRequestTime + 10 < tNow )
	{
		mutex.unlock();
		SanityCheck();
		mutex.relock();
	}
	else if ( m_nLastSettingsLoadTime + m_nSettingsLoadInterval < tNow )
	{
		LoadSettings( tNow );
	}
	else if ( m_nLastRuleExpiryCheck + m_nRuleExpiryInterval < tNow )
	{
		Expire();
	}
	else if ( m_nLastMissCacheExpiryCheck + m_nMissCacheExpiryInterval < tNow )
	{
		mutex.unlock();
		MissCacheClear( tNow );
		mutex.relock();
	}

	// First, check the miss cache if the IP is not included in the list of rules.
	// If the address is in cache, it is a miss and no further lookup is needed.
	if ( m_bUseMissCache && m_Cache.count( pAddress->toString() ) )
	{
		if ( m_bDebug )
		{
//			theApp.Message( MSG_DEBUG, "Skipped  repeat IP security check for %s (%i IPs cached; Call source: %s)",
//							pAddress->toString(), m_Cache.size(), source );
		}

		return m_bDenyPolicy;
	}
	else if ( m_bDebug )
	{
//		theApp.Message( MSG_DEBUG, "Called first-time IP security check for %s (Call source: %s)"),
//						pAddress->toString(), source );
	}

	// Second, check the fast IP rules lookup map.
	CAddressRuleMap::const_iterator i;
	i = m_IPs.find( pAddress->toString() );

	if ( i != m_IPs.end() )
	{
		CSecureRule* pIPRule = (*i).second;
		if ( pIPRule->IsExpired( tNow ) )
		{
			mutex.unlock();
			Remove( pIPRule );
			mutex.relock();
		}
		else
		{
			pIPRule->Count();

			if ( pIPRule->m_nExpire > CSecureRule::srSession && pIPRule->m_nExpire < tNow + 300 )
			{	// Add 5 min penalty for early access
				pIPRule->m_nExpire = tNow + 300;
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

		if ( pRule->Match( pAddress ) )
		{
			if ( pRule->IsExpired( tNow ) )
			{
				mutex.unlock();
				Remove( pRule ); // invalidates iterator
				mutex.relock();

				it = m_IPRanges.begin();
				continue;
			}

			pRule->Count();

			if ( pRule->m_nExpire > CSecureRule::srSession && pRule->m_nExpire < tNow + 300 )
				// Add 5 min penalty for early access
				pRule->m_nExpire = tNow + 300;

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
	MissCacheAdd ( pAddress->toString() );

	// In this case, return our default policy
	return m_bDenyPolicy;
}

/*bool CSecurity::IsDenied(const CQueryHit* pHit, const CQuerySearch* pQuery)
{
	return ( IsDenied( (CShareazaFile*)pHit ) || IsDenied( pHit->m_sName ) || IsDenied( pQuery, pHit->m_sName ) );
}*/

// If the remote computer is running a client that is breaking GPL, causing problems, etc.
// We don't actually ban these clients, but we don't accept them as a leaf. Can still upload, though.
bool CSecurity::IsClientBad(const QString& sUserAgent) const
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
bool CSecurity::IsAgentBlocked(const QString& sUserAgent)
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
	return IsAgentDenied( sUserAgent );
}

bool CSecurity::IsVendorBlocked(const QString& sVendor) const
{
	// foxy - leecher client. (Tested, does not upload)
	// having something like Authentication which is not defined on specification
	if ( sVendor.startsWith( "foxy" ) )						return true;

	// Allow it
	return false;
}

//////////////////////////////////////////////////////////////////////
// CSecurity load and save
void CSecurity::LoadSettings(const quint32 tNow)
{
	QMutexLocker l( &m_pSection );

	m_bDebug = true; //Settings.Security.DebugLogIPMissCacheHits;
	m_nRuleExpiryInterval = 600; //Settings.Security.RuleExpiryCheckInterval;
	m_nMissCacheExpiryInterval = 600; //Settings.Security.MissCacheExpiryInterval;

	m_nLastSettingsLoadTime = tNow;
}

bool CSecurity::Load()
{
	LoadSettings( static_cast< quint32 >( time( NULL ) ) );

	QMutexLocker mutex( &m_pSection );

//	CFile pFile;
//	QString strFile = Settings.General.UserPath + "\\Data\\Security.dat";

//	if ( ! pFile.Open( strFile, CFile::modeRead ) )
//		return false;

	try
	{
//		CArchive ar( &pFile, CArchive::load, 131072 );	// 128 KB buffer
//		Serialize( ar );
//		ar.Close();
	}
	catch ( ... /*CException* pException*/ )
	{
		mutex.unlock();

//		pException->Delete();

		Clear();
//		pFile.Abort();
		m_bIsLoading = false;

		return false;
	}

//	pFile.Close();

	return true;
}

bool CSecurity::Save()
{
	QMutexLocker l( &m_pSection );

	if ( m_bSaved )		// Saving not required ATM
	{
		return true;
	}

//	CFile pFile;
//	QString strFile = Settings.General.UserPath + "\\Data\\Security.dat";

//	if ( ! pFile.Open( strFile, CFile::modeWrite|CFile::modeCreate ) )
//		return false;

	try
	{
//		CArchive ar( &pFile, CArchive::store, 131072 );	// 128 KB buffer
		try
		{
//			Serialize( ar );
//			ar.Close();
		}
		catch ( ... /*CException* pException*/ )
		{
//			ar.Abort();
//			pFile.Abort();
//			pException->Delete();
			return false;
		}
//		pFile.Close();
	}
	catch ( ... /*CException* pException*/ )
	{
//		pFile.Abort();
//		pException->Delete();
		return false;
	}

	m_bSaved = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
// CSecurity import

// todo: rewrite completely
/*bool CSecurity::Import(LPCTSTR pszFile)
{
	QString strText;
	CBuffer pBuffer;
	CFile pFile;

	if ( ! pFile.Open( pszFile, CFile::modeRead ) )
		return false;

	pBuffer.EnsureBuffer( (quint32)pFile.GetLength() );
	pBuffer.m_nLength = (quint32)pFile.GetLength();
	pFile.Read( pBuffer.m_pBuffer, pBuffer.m_nLength );
	pFile.Close();

	CXMLElement* pXML = CXMLElement::FromBytes( pBuffer.m_pBuffer, pBuffer.m_nLength, TRUE );

	bool bResult = false;

	if ( pXML != NULL )
	{
		// Importing XML file
		bResult = FromXML( pXML );
		delete pXML;
	}
	else
	{
		// Importing Gnucleus security file
		bResult = FromGnucleus( pBuffer );
	}

	// Check all lists for newly denied hosts
	SanityCheck();

	return bResult;
}*/

//////////////////////////////////////////////////////////////////////
// CSecurity XML

const QString CSecurity::xmlns = "http://www.shareaza.com/schemas/Security.xsd";

/*CXMLElement* CSecurity::ToXML()
{
	CXMLElement* pXML = new CXMLElement( NULL, "security" );
	pXML->AddAttribute( "xmlns", CSecurity::xmlns );

	for ( CIterator i = GetIterator(); i != GetEnd() ; i++ )
	{
		// Invalid rules could return NULL.
		if ( CXMLElement* pElement = (*i)->ToXML() )
		{
			pXML->AddElement( pElement );
		}
	}

	return pXML;
}*/

/*bool CSecurity::FromXML(CXMLElement* pXML)
{
	if ( ! pXML->IsNamed( "security" ) )
		return false;

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

//	CQuickLock oLock( m_pSection );

	unsigned int size = GetCount() + pXML->GetElementCount();
	size = size + size / 4u;
	if ( m_pLiveList->GetAllocatedSize() != GetBestHashTableSize( size ) )
		GenerateList();

	int nCount = 0;
	m_bIsLoading = true;

	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pElement = pXML->GetNextElement( pos );

		if ( pElement->IsNamed( "rule" ) )
		{
			CSecureRule* pRule	= CSecureRule::FromXML( pElement );

			if ( pRule )
			{
				if ( GetUUID( pRule->m_oUUID ) == GetEnd() && !pRule->IsExpired( tNow ) )
				{
					Add( pRule );
				}
				else
				{
					delete pRule;
				}
				nCount++;
			}
			else
			{
				theApp.Message( MSG_ERROR, IDS_SECURITY_XML_RULE_IMPORT_FAIL );
			}
		}
	}

	m_bIsLoading = false;

	return nCount > 0;
}*/

//////////////////////////////////////////////////////////////////////
// Private method definitions

/*void CSecurity::Serialize(CArchive& ar)
{
	int nVersion = SECURITY_SER_VERSION;

	CSecureRule* pRule = NULL;

	if ( ar.IsStoring() )
	{
		TRACE( " >> Security Version : %d\n", nVersion );
		ar << nVersion;

		TRACE( " >> Deny Policy      : %s\n", m_bDenyPolicy ? "Deny" : "Allow" );
		bool nDenyPolicy = m_bDenyPolicy;	// BOOL is used for compatibility with old security
		ar << nDenyPolicy;					// code to avoid problems while up- or downgrading.

		INT_PTR nCount = GetCount();
		TRACE( " >> Rule Count       : %d\n", nCount );
		ar.WriteCount( nCount );

		for ( CIterator i = GetIterator(); i != GetEnd(); i++ )
		{
			pRule = *i;
			CSecureRule::Serialize( pRule, ar, nVersion );

			nCount--;
		}
	}
	else // archive is
	{
		Clear();

		ar >> nVersion;
		TRACE( " >> Security Version : %d\n", nVersion );
		bool nDenyPolicy;	// BOOL is used for compatibility with old security
		ar >> nDenyPolicy;	// code to avoid problems while up- or downgrading.
		m_bDenyPolicy = ( nDenyPolicy != 0 );
		TRACE( " >> Deny Policy      : %s\n", m_bDenyPolicy ? "Deny" : "Allow" );

		quint32 tNow = static_cast< quint32 >( time( NULL ) );

		quint32_PTR nCount = ar.ReadCount();
		TRACE( " >> Rule Count       : %d\n", nCount );

		unsigned int size = GetBestHashTableSize( nCount + nCount / 4u );

		Q_ASSERT_VALID( m_pLiveList );
		delete m_pLiveList;
		// make sure the list size does not have to be reajusted while loading the security rules from file.
		m_pLiveList = new CLiveList( 6, size );

		RefreshDefaultPolicyItem();

		m_bIsLoading = true;

		while ( nCount > 0 )
		{
			CSecureRule::Serialize( pRule, ar, nVersion );

			if ( pRule->IsExpired( tNow, true ) )
			{
				delete pRule;
			}
			else
			{
				Add( pRule );
			}

			pRule = NULL;

			nCount--;
		}

		m_bIsLoading = false;

		// if necessary perform sanity check after loading
		SanityCheck();
	}
}*/

CSecurity::CIterator CSecurity::GetHash(const QString& sSHA1,
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
	BYTE count = bSHA1 + bED2K + bBTIH + bTTH + bMD5;

	// We are not searching for any hash. :)
	if ( !count ) return GetEnd();

	CHashRuleMap::const_iterator i;

	i = m_Hashes.find( sSHA1 );

	while ( i != m_Hashes.end() )
	{
		if ( (*i).second->Match(sSHA1, sED2K, sTTH, sBTIH, sMD5) ) return GetUUID( (*i).second->m_oUUID );
		i++;

		if( (*i).first != sSHA1 ) break;
	}

	i = m_Hashes.find( sED2K );

	while ( i != m_Hashes.end() )
	{
		if ( (*i).second->Match(sSHA1, sED2K, sTTH, sBTIH, sMD5) ) return GetUUID( (*i).second->m_oUUID );
		i++;

		if ( (*i).first != sED2K ) break;
	}

	i = m_Hashes.find( sBTIH );

	while ( i != m_Hashes.end() )
	{
		if ( (*i).second->Match(sSHA1, sED2K, sTTH, sBTIH, sMD5) ) return GetUUID( (*i).second->m_oUUID );
		i++;

		if ( (*i).first != sBTIH ) break;
	}

	i = m_Hashes.find( sTTH );

	while ( i != m_Hashes.end() )
	{
		if ( (*i).second->Match(sSHA1, sED2K, sTTH, sBTIH, sMD5) ) return GetUUID( (*i).second->m_oUUID );
		i++;

		if ( (*i).first != sTTH ) break;
	}

	i = m_Hashes.find( sMD5 );

	while ( i != m_Hashes.end() )
	{
		if ( (*i).second->Match(sSHA1, sED2K, sTTH, sBTIH, sMD5) ) return GetUUID( (*i).second->m_oUUID );
		i++;

		if ( (*i).first != sMD5 ) break;
	}

	return GetEnd();
}
CSecurity::CIterator CSecurity::GetUUID(const QUuid& oUUID) const
{
	for ( CIterator i = GetIterator() ; i != GetEnd(); i++ )
	{
		if ( (*i)->m_oUUID == oUUID ) return i;
	}

	return GetEnd();
}

CSecurity::CIterator CSecurity::Remove(CIterator it)
{
	if ( it == GetEnd() )
		return it;

	CSecureRule* pRule = *it;
	CSecureRule::RuleType type = pRule->Type();

	// Removing the rule from special containers for fast access.
	switch ( type )
	{
	case CSecureRule::srContentAddress:
		{
			quint32 nIP = ((CIPRule*)pRule)->m_oIP;
			CAddressRuleMap::iterator i = m_IPs.find( nIP );

			if ( i != m_IPs.end() && (*i).second->m_oUUID == pRule->m_oUUID )
			{
				m_IPs.erase( i );

				if ( m_bUseMissCache )
					EvaluateCacheUsage();
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

			if( pRule->m_nAction == CSecureRule::srAccept )
				MissCacheClear( static_cast< quint32 >( time( NULL ) ) );

			if ( m_bUseMissCache )
				EvaluateCacheUsage();
		}
		break;

	case CSecureRule::srContentCountry:
		{
			QString country = ((CCountryRule*)pRule)->GetContent();
			CCountryRuleMap::iterator i = m_Countries.find( country );

			if ( i != m_Countries.end() && (*i).second->m_oUUID == pRule->m_oUUID )
			{
				m_Countries.erase( i );

				if ( m_bUseMissCache )
					EvaluateCacheUsage();
			}
		}
		break;

	case CSecureRule::srContentHash:
		{
			CHashRule* pHashRule = (CHashRule*)pRule;

			QString sSHA1 = pHashRule->GetSHA1urn();
			QString sED2K = pHashRule->GetED2Kurn();
			QString sBTIH = pHashRule->GetBTIHurn();
			QString sTTH  = pHashRule->GetTTHurn();
			QString sMD5  = pHashRule->GetMD5urn();

			CHashRuleMap::const_iterator i;

			i = m_Hashes.find( sSHA1 );

			while ( i != m_Hashes.end() && (*i).second->GetSHA1urn() == sSHA1 )
			{
				if ( (*i).second->m_oUUID == pHashRule->m_oUUID )
				{
					i = m_Hashes.erase( i );
				}
				else
				{
					i++;
				}
			}

			i = m_Hashes.find( sED2K );

			while ( i != m_Hashes.end() && (*i).second->GetED2Kurn() == sED2K )
			{
				if ( (*i).second->m_oUUID == pHashRule->m_oUUID )
				{
					i = m_Hashes.erase( i );
				}
				else
				{
					i++;
				}
			}

			i = m_Hashes.find( sBTIH );

			while ( i != m_Hashes.end() && (*i).second->GetBTIHurn() == sBTIH )
			{
				if ( (*i).second->m_oUUID == pHashRule->m_oUUID )
				{
					i = m_Hashes.erase( i );
				}
				else
				{
					i++;
				}
			}

			i = m_Hashes.find( sTTH );

			while ( i != m_Hashes.end() && (*i).second->GetTTHurn() == sTTH )
			{
				if ( (*i).second->m_oUUID == pHashRule->m_oUUID )
				{
					i = m_Hashes.erase( i );
				}
				else
				{
					i++;
				}
			}

			i = m_Hashes.find( sMD5 );

			while ( i != m_Hashes.end() && (*i).second->GetMD5urn() == sMD5 )
			{
				if ( (*i).second->m_oUUID == pHashRule->m_oUUID )
				{
					i = m_Hashes.erase( i );
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

	delete pRule;

	m_bSaved = false;

	// Remove rule entry from list of all rules
	return m_Rules.erase( it );
}

bool CSecurity::IsAgentDenied(const QString& strUserAgent)
{
	if ( strUserAgent.isEmpty() )
		return false;

	// Private member function. Only called from places that have already a lock.

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	CUserAgentRuleMap::iterator i = m_UserAgents.find( strUserAgent );
	if ( i != m_UserAgents.end() )
	{
		CUserAgentRule* pRule = (*i).second;

		if ( pRule->Match( strUserAgent ) )
		{
			if ( pRule->IsExpired( tNow ) )
			{
				Remove( pRule );
			}

			pRule->Count();

			if ( pRule->m_nExpire > CSecureRule::srSession &&
				pRule->m_nExpire < tNow + 300 )
				// Add 5 min penalty for early access
				pRule->m_nExpire = tNow + 300;

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}
	}

	return false;
}

void CSecurity::MissCacheAdd( const QString &sIP )
{
	if ( m_bUseMissCache )
	{
		m_Cache.insert( sIP );
		EvaluateCacheUsage();
	}
}
void CSecurity::MissCacheClear( const quint32 &tNow )
{
	m_Cache.clear();
	m_nLastMissCacheExpiryCheck = tNow;

	if ( !m_bUseMissCache )
		EvaluateCacheUsage();
}
void CSecurity::EvaluateCacheUsage()
{
	double nCache		= m_Cache.size();
	double nIPMap		= m_IPs.size();
	double nCountryMap	= m_Countries.size();

	if ( ! m_Cache.size() )		nCache = 1;
	if ( ! m_IPs.size() )		nIPMap = 1;
	if ( ! m_Countries.size() )	nCountryMap = 1;

	m_bUseMissCache = ( log( nCache ) < ( log( nIPMap * nCountryMap ) + m_IPRanges.size() * log( 2.0 ) ) );
}

bool CSecurity::IsDenied(const QString& strContent)
{
	if ( strContent.isEmpty() )
		return false;

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	QMutexLocker mutex( &m_pSection );

	CContentRuleList::iterator i = m_Contents.begin();
	while ( i != m_Contents.end() )
	{
		CContentRule* pRule = *i;

		if ( pRule->Match( strContent ) )
		{
			if ( pRule->IsExpired( tNow ) )
			{
				mutex.unlock();
				Remove( pRule );
				mutex.relock();

				i = m_Contents.begin();
				continue;
			}

			pRule->Count();

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
}
/*bool CSecurity::IsDenied(const CShareazaFile* pFile)
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
	CIterator it = GetHash( sSHA1, sED2K, sTTH, sBTIH, sMD5 );

	// If this rule matches the file, return denied.
	if ( it != GetEnd() && ((CHashRule*)*it)->Match( sSHA1, sED2K, sTTH, sBTIH, sMD5 ) )
		return true;

	// Else check other content rules.
	CContentRuleList::iterator i = m_Contents.begin();
	while ( i != m_Contents.end() )
	{
		CContentRule* pRule = *i;

		if ( pRule->Match( pFile ) )
		{
			if ( pRule->IsExpired( tNow ) )
			{
				mutex.unlock();
				Remove( pRule );
				mutex.relock();

				i = m_Contents.begin();
				continue;
			}

			pRule->Count();

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
/*bool CSecurity::IsDenied(const CQuerySearch* pQuery, const QString& strContent)
{
	if ( !pQuery || strContent.isEmpty() )
		return false;

	quint32 tNow = static_cast< quint32 >( time( NULL ) );

	QMutexLocker mutex( &m_pSection );

	CRegExpRuleList::iterator i = m_RegExpressions.begin();
	while ( i != m_RegExpressions.end() )
	{
		CRegExpRule* pRule = *i;

		if ( pRule->Match( pQuery, strContent ) )
		{
			if ( pRule->IsExpired( tNow ) )
			{
				mutex.unlock();
				Remove( pRule );
				mutex.relock();

				i = m_RegExpressions.begin();

				continue;
			}

			pRule->Count();

			if ( pRule->m_nAction == CSecureRule::srAccept )
				return false;
			else if ( pRule->m_nAction == CSecureRule::srDeny )
				return true;
		}

		i++;
	}

	return false;
}*/

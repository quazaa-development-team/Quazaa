#include "security.h"
#include "securerule.h"

using namespace security;

CSecureRule::CSecureRule(bool bCreate)
{
	m_nType		= srContentUndefined; // invalidates rule as long as it does not contain any useful content.
	m_nAction	= srDeny;
	m_sComment	= "";
	m_nExpire	= srIndefinite;
	m_nToday	= 0;
	m_nTotal	= 0;

	if ( bCreate ) m_oUUID.createUuid();
}

CSecureRule& CSecureRule::operator=(const CSecureRule& pRule)
{
	m_nType		= pRule.m_nType;
	m_nAction	= pRule.m_nAction;
	m_sComment	= pRule.m_sComment;
	m_oUUID		= pRule.m_oUUID;
	m_nExpire	= pRule.m_nExpire;
	m_nToday	= pRule.m_nToday;
	m_nTotal	= pRule.m_nTotal;

	return *this;
}

//////////////////////////////////////////////////////////////////////
// CSecureRule expiry check

bool CSecureRule::IsExpired(quint32 tNow, bool bSession) const
{
	if ( m_nExpire == srIndefinite ) return false;
	if ( m_nExpire == srSession ) return bSession;
	return m_nExpire < tNow;
}

CSecureRule* CSecureRule::GetCopy() const
{
	// This method should never be called.
#ifdef _DEBUG
	ASSERT( false );
#endif //_DEBUG

	return new CSecureRule( *this );
}

//////////////////////////////////////////////////////////////////////
// CSecureRule match

bool CSecureRule::Match(const QHostAddress*) const
{
	return false;
}
bool CSecureRule::Match(const QString&) const
{
	return false;
}
/*bool CSecureRule::Match(const CShareazaFile*) const
{
	return false;
}
bool CSecureRule::Match(const CQuerySearch*, const QString&) const
{
	return false;
}*/

//////////////////////////////////////////////////////////////////////
// CSecureRule serialize

/*void CSecureRule::Serialize(CSecureRule* &sr, CArchive& ar, int nVersion)
{
	QString strTemp;

	if ( ar.IsStoring() )
	{
		ar << (int)(sr->m_nType);
		ar << sr->m_nAction;
		ar << sr->m_sComment;

		ar.Write( &(sr->m_oUUID), sizeof(GUID) );

		ar << sr->m_nExpire;
		ar << sr->m_nTotal;

		switch ( sr->m_nType )
		{
		case srContentAddress:
			ar.Write( &((CIPRule*)sr)->m_oIP, 4 );
			break;
		case srContentAddressRange:
			ar.Write( ((CIPRangeRule*)sr)->m_oIP, 4 );
			ar.Write( ((CIPRangeRule*)sr)->m_nMask, 4 );
			break;
		case srContentHash:
			strTemp = ((CHashRule*)sr)->GetContentWords();
			ar << strTemp;
			break;
		case srContentAny:
		case srContentAll:
			strTemp = ((CContentRule*)sr)->GetContentWords();
			ar << strTemp;
			break;
		case srContentRegExp:
			strTemp = ((CRegExpRule*)sr)->GetContentWords();
			ar << strTemp;
			break;
		case srContentUserAgent:
			strTemp = ((CUserAgentRule*)sr)->GetContentWords();
			ar << strTemp;
			break;
		default:
			// Do nothing. We don't store invalid rules.
			break;
		}
	}
	else // read from archive
	{
		int			nType;
		BYTE		action;
		QString		comment;
		GUID		GUID;
		quint32		expire;
		quint32		total;


		ar >> nType;
		ar >> action;

		if ( nVersion >= 2 )
			ar >> comment;

		if ( nVersion >= 4 )
			ReadArchive( ar, &GUID, sizeof(GUID) );
		else
			CoCreateGuid( &GUID );

		ar >> expire;
		ar >> total;

		CSecureRule* pRule = NULL;

		if( nVersion > 5 )
		{
			switch ( nType )
			{
			case 0:
				// contentless rule
				pRule = new CSecureRule();
				break;
			case 1:
				pRule = new CIPRule();
				break;
			case 2:
				pRule = new CIPRangeRule();
				break;
			case 3:
				pRule = new CCountryRule();
				break;
			case 4:
				pRule = new CHashRule();
				break;
			case 5:
				pRule = new CContentRule();
				break;
			case 6:
				pRule = new CContentRule();
				break;
			case 7:
				pRule = new CRegExpRule();
				break;
			case 8:
				pRule = new CUserAgentRule();
				break;
			default:
				ASSERT( false );
				pRule = new CSecureRule();
				break;
			}

			if ( pRule->m_nType == srContentAddress )
			{
				ReadArchive( ar, &((CIPRangeRule*)pRule)->m_oIP, 4 );
			}
			else if ( pRule->m_nType == srContentAddressRange )
			{
				ReadArchive( ar, ((CIPRangeRule*)pRule)->m_oIP, 4 );
				ReadArchive( ar, ((CIPRangeRule*)pRule)->m_nMask, 4 );
			}
			else if ( pRule->m_nType == srContentAll || pRule->m_nType == srContentAny )
			{
				ar >> strTemp;
				((CContentRule*)pRule)->SetContentWords( strTemp );
			}
			else if ( pRule->m_nType == srContentHash )
			{
				ar >> strTemp;
				((CHashRule*)pRule)->SetContentWords( strTemp );
			}
			else if ( pRule->m_nType == srContentRegExp )
			{
				ar >> strTemp;
				((CRegExpRule*)pRule)->SetContentWords( strTemp );
			}
			else if (pRule->m_nType == srContentUserAgent )
			{
				ar >> strTemp;
				((CUserAgentRule*)pRule)->SetContentWords( strTemp );
			}
			else
			{
				// There is an empty or erroneous rule. Error handling (if necessary) should go here.
				theApp.Message( MSG_ERROR, IDS_SECURITY_ARCHIVE_RULE_LOAD_FAIL );
			}
		}
		// This handles upgrades from version 5 (and previous versions) of the security code.
		else
		{
			RuleType type = srContentUndefined;

			switch ( nType )
			{
			case 0:
				type = srContentAddress;
				break;
			case 1:
				type = srContentAny;
				break;
			case 2:
				type = srContentAll;
				break;
			case 3:
				type = srContentRegExp;
				break;
			}

			if ( type == srContentAddress )
			{
				BYTE pIP[4], pMask[4];
				ReadArchive( ar, pIP, 4 );
				ReadArchive( ar, pMask, 4 );

				if ( *(quint32*)pMask == 0xFFFFFFFF )
				{
					pRule = new CIPRule();
					((CIPRule*)pRule)->m_oIP = *(quint32*)pIP;
				}
				else
				{
					pRule = new CIPRangeRule();
					CopyMemory(((CIPRangeRule*)pRule)->m_oIP, pIP, sizeof( quint32 ) );
					CopyMemory(((CIPRangeRule*)pRule)->m_nMask, pMask, sizeof( quint32 ) );
					((CIPRangeRule*)pRule)->MaskFix();		// Make sure old rules are updated to new format
				}
			}
			else
			{
				strTemp = "";
				if ( nVersion < 5 )
				{
					ASSERT( type == srContentAny );

					BYTE foo;
					ar >> foo;
					switch ( foo )
					{
					case 1:
						type = srContentAll;
						break;
					case 2:
						type = srContentRegExp;
						break;
					}
				}

				if ( nVersion < 3 )
				{
					for ( quint32_PTR nCount = ar.ReadCount() ; nCount > 0 ; nCount-- )
					{
						QString strWord;
						ar >> strWord;

						strTemp += ' ';
						strTemp += strWord;
					}
				}
				else
				{
					ar >> strTemp;
				}

				if ( type == srContentRegExp )
				{
					pRule = new CRegExpRule();
					((CRegExpRule*)pRule)->SetContentWords( strTemp );
				}
				// Before version 6, this also included hash rules.
				else if ( type == srContentAny || type == srContentAll )
				{
					QString strUrn = strTemp.mid( 0, 4 );

					if ( strUrn.CompareNoCase( _T("urn:") ) == 0 )
					{
						CHashRule* rule = new CHashRule();
						rule->SetContentWords( strTemp );

						pRule = rule;
					}
					else
					{
						CContentRule* rule = new CContentRule();
						rule->SetContentWords( strTemp );

						rule->SetAll( type == srContentAll );

						pRule = rule;
					}
				}
				else
				{
					// There is an erroneous rule. Error handling (if necessary) should go here.
					theApp.Message( MSG_ERROR, IDS_SECURITY_ARCHIVE_RULE_LOAD_FAIL );
				}
			}
		}

		pRule->m_nAction  = action;
		pRule->m_sComment = comment;
		pRule->m_oUUID    = GUID;
		pRule->m_nExpire  = expire;
		pRule->m_nTotal   = total;

		if( sr )
		{
			delete sr;
			sr = NULL;
		}
		sr = pRule;
	}
}*/

//////////////////////////////////////////////////////////////////////
// CSecureRule XML

/*CXMLElement* CSecureRule::ToXML() const
{
	// As this method is virtual and the main class should never be used directly,
	// we got a problem if this method is ever called.

#ifdef _DEBUG
	ASSERT( false );
#endif //_DEBUG

	return NULL;
}*/

// Contains default code for XML generation.
/*void CSecureRule::ToXML(const CSecureRule* const &rule, CXMLElement* &pXML)
{
	if ( !( rule->m_sComment.isEmpty() ) )
	{
		pXML->AddAttribute( _T("comment"), rule->m_sComment );
	}

	switch ( rule->m_nAction )
	{
	case srNull:
		pXML->AddAttribute( _T("action"), _T("null") );
		break;
	case srAccept:
		pXML->AddAttribute( _T("action"), _T("accept") );
		break;
	case srDeny:
		pXML->AddAttribute( _T("action"), _T("deny") );
		break;
	}

	if ( rule->m_nExpire == srSession )
	{
		pXML->AddAttribute( _T("expire"), _T("session") );
	}
	else if ( rule->m_nExpire == srIndefinite )
	{
		pXML->AddAttribute( _T("expire"), _T("indefinite") );
	}
	else if ( rule->m_nExpire > srSession )
	{
		QString strValue;
		strValue.Format( _T("%lu"), rule->m_nExpire );
		pXML->AddAttribute( _T("expire"), strValue );
	}

	wchar_t szGUID[39];
	StringFromGUID2( *(GUID*)&( rule->m_oUUID ), szGUID, 39 );
	QString strGUID( &szGUID[1] );
	strGUID.Delete( 37 );
	pXML->AddAttribute( _T("guid"), strGUID );
}*/

/*CSecureRule* CSecureRule::FromXML(CXMLElement* pXML)
{
	CSecureRule* pRule = NULL;

	QString strType = pXML->GetAttributeValue( _T("type") );

	if ( strType.CompareNoCase( _T("address") ) == 0 )
	{
		int x[4];

		BYTE pIP[4] = { (BYTE)0 };
		BYTE mask[4] = { (BYTE)0 };
		bool IPSet = false;
		bool maskSet = false;

		QString strAddress = pXML->GetAttributeValue( _T("address") );
		if ( _stscanf( strAddress, _T("%lu.%lu.%lu.%lu"), &x[0], &x[1], &x[2], &x[3] ) == 4 )
		{
			pIP[0] = (BYTE)x[0];
			pIP[1] = (BYTE)x[1];
			pIP[2] = (BYTE)x[2];
			pIP[3] = (BYTE)x[3];

			IPSet = true;
		}

		QString strMask = pXML->GetAttributeValue( _T("mask") );
		if ( _stscanf( strMask, _T("%lu.%lu.%lu.%lu"), &x[0], &x[1], &x[2], &x[3] ) == 4 )
		{
			mask[0] = (BYTE)x[0];
			mask[1] = (BYTE)x[1];
			mask[2] = (BYTE)x[2];
			mask[3] = (BYTE)x[3];

			maskSet = ( *(quint32*)mask != 0xFFFFFFFF );
		}

		if( IPSet )
		{
			if( maskSet )
			{
				CIPRangeRule* rule = new CIPRangeRule();

				rule->m_oIP[0] = pIP[0];
				rule->m_oIP[1] = pIP[1];
				rule->m_oIP[2] = pIP[2];
				rule->m_oIP[3] = pIP[3];

				rule->m_nMask[0] = mask[0];
				rule->m_nMask[1] = mask[1];
				rule->m_nMask[2] = mask[2];
				rule->m_nMask[3] = mask[3];

				rule->MaskFix();

				pRule = rule;
			}
			else
			{
				CIPRule* rule = new CIPRule();

				rule->m_oIP = *(quint32*) pIP;
				pRule = rule;
			}
		}
		else
		{
			// Error in XML entry.
			return NULL;
		}
	}
	else if ( strType.CompareNoCase( _T("hash") ) == 0 )
	{
		CHashRule* rule = new CHashRule();
		rule->SetContentWords( pXML->GetAttributeValue( _T("content") ) );

		if ( rule->GetContentWords().Trim().isEmpty() )
		{
			delete rule;
			return NULL;
		}

		pRule = rule;
	}
	else if ( strType.CompareNoCase( _T("regexp") ) == 0 )
	{
		CRegExpRule* rule = new CRegExpRule();
		rule->SetContentWords( pXML->GetAttributeValue( _T("content") ) );

		if ( rule->GetContentWords().Trim().isEmpty() )
		{
			delete rule;
			return NULL;
		}

		pRule = rule;
	}
	else if ( strType.CompareNoCase( _T("content") ) == 0 )
	{
		QString strMatch = pXML->GetAttributeValue( _T("match") );
		QString strContent = pXML->GetAttributeValue( _T("content") );

		QString strUrn = strContent.left( 4 );

		// handles "old style" regexp rules
		if ( strMatch.CompareNoCase( _T("regexp") ) == 0 )
		{
			CRegExpRule* rule = new CRegExpRule();
			rule->SetContentWords( strContent );

			if ( rule->GetContentWords().Trim().isEmpty() )
			{
				delete rule;
				return NULL;
			}

			pRule = rule;
		}
		// handles "old style" hash rules
		else if ( strUrn.CompareNoCase( _T("urn:") ) == 0 )
		{
			CHashRule* rule = new CHashRule();
			rule->SetContentWords( pXML->GetAttributeValue( _T("content") ) );

			if ( rule->GetContentWords().Trim().isEmpty() )
			{
				delete rule;
				return NULL;
			}

			pRule = rule;
		}
		else
		{
			bool all = ( strMatch.CompareNoCase( _T("all") ) == 0 );

			if ( all || strMatch.CompareNoCase( _T("any") ) == 0 )
			{
				CContentRule* rule = new CContentRule();
				rule->SetAll( all );
				rule->SetContentWords( pXML->GetAttributeValue( _T("content") ) );

				if ( rule->GetContentWords().Trim().isEmpty() )
				{
					delete rule;
					return NULL;
				}

				pRule = rule;
			}
			else
			{
				return NULL;
			}
		}
	}
	else if ( strType.CompareNoCase( _T("country") ) == 0 )
	{
		CCountryRule* rule = new CCountryRule();
		rule->SetContent	( pXML->GetAttributeValue( _T("content") ) );

		if ( rule->GetContent().Trim().isEmpty() )
		{
			delete rule;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}

	QString strAction = pXML->GetAttributeValue( _T("action") );

	if ( strAction.CompareNoCase( _T("null") ) == 0 )
	{
		pRule->m_nAction = srNull;
	}
	else if ( strAction.CompareNoCase( _T("accept") ) == 0 )
	{
		pRule->m_nAction = srAccept;
	}
	else if ( strAction.CompareNoCase( _T("deny") ) == 0 || strAction.isEmpty() )
	{
		pRule->m_nAction = srDeny;
	}
	else
	{
		delete pRule;
		return NULL;
	}

	pRule->m_sComment = pXML->GetAttributeValue( _T("comment") );

	QString strExpire = pXML->GetAttributeValue( _T("expire") );
	if ( strExpire.CompareNoCase( _T("session") ) == 0 )
	{
		pRule->m_nExpire = srSession;
	}
	else if ( strExpire.CompareNoCase( _T("indefinite") ) == 0 )
	{
		pRule->m_nExpire = srIndefinite;
	}
	else
	{
		_stscanf( strExpire, _T("%lu"), &(pRule->m_nExpire) );
	}

	QString strGUID = pXML->GetAttributeValue( _T("guid") );
	GUID pGUID;

	if ( Hashes::fromGuid( strGUID, &pGUID ) )
	{
		pRule->m_oUUID = pGUID;
	}
	else
	{
		CoCreateGuid( &(pRule->m_oUUID) );
	}

	return pRule;
}*/

//////////////////////////////////////////////////////////////////////
// CIPRule

CIPRule::CIPRule(bool)
{
	m_nType = srContentAddress;
}

CIPRule& CIPRule::operator=(const CIPRule& pRule)
{
#ifdef _DEBUG
	ASSERT( &pRule && pRule.m_nType == srContentAddress );
#endif //_DEBUG

	this->CSecureRule::operator=( pRule );

	m_oIP = pRule.m_oIP;

	return *this;
}

bool CIPRule::Match(const QHostAddress* pAddress) const
{
#ifdef _DEBUG
	ASSERT( pAddress && m_nType == srContentAddress );
#endif //_DEBUG

	if ( pAddress && ( *pAddress == m_oIP ) )
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
// CIPRangeRule

CIPRangeRule::CIPRangeRule(bool)
{
	m_nMask = 0;

	m_nType = srContentAddressRange;
}

CIPRangeRule& CIPRangeRule::operator=(const CIPRangeRule& pRule)
{
#ifdef _DEBUG
	ASSERT( &pRule && pRule.m_nType == srContentAddressRange );
#endif //_DEBUG

	this->CSecureRule::operator=( pRule );

	m_oIP   = pRule.m_oIP;
	m_nMask = pRule.m_nMask;

	return *this;
}

bool CIPRangeRule::Match(const QHostAddress* pAddress) const
{
#ifdef _DEBUG
	ASSERT( pAddress && m_nType == srContentAddressRange );
#endif //_DEBUG

	return pAddress;// && ( pAddress->s_addr & *(quint32*)m_nMask ) == *(quint32*)m_oIP;
}

//////////////////////////////////////////////////////////////////////
// CCountryRule

CCountryRule::CCountryRule(bool)
{
	m_sCountry = "";
}

CCountryRule& CCountryRule::operator=(const CCountryRule& pRule)
{
#ifdef _DEBUG
	ASSERT( &pRule && pRule.m_nType == srContentCountry );
#endif //_DEBUG

	this->CSecureRule::operator=( pRule );

	m_sCountry = pRule.m_sCountry;

	return *this;
}

bool CCountryRule::Match(const QHostAddress* pAddress) const
{
#ifdef _DEBUG
	ASSERT( pAddress && m_nType == srContentCountry );
#endif //_DEBUG

	if ( pAddress /*&& theApp.GetCountryCode( *pAddress ) == m_sCountry*/ )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////
// CHashRule

CHashRule::CHashRule(bool)
{
	m_sSHA1  = "";
	m_sED2K  = "";
	m_sBTIH  = "";
	m_sTTH   = "";
	m_sMD5   = "";

	m_nHashWeight = 1;
	m_nType = srContentHash;
}

CHashRule& CHashRule::operator=(const CHashRule& pRule)
{
#ifdef _DEBUG
	ASSERT( &pRule && pRule.m_nType == srContentHash );
#endif //_DEBUG

	this->CSecureRule::operator=( pRule );

	m_sSHA1 = pRule.m_sSHA1;
	m_sED2K = pRule.m_sED2K;
	m_sBTIH = pRule.m_sBTIH;
	m_sTTH  = pRule.m_sTTH;
	m_sMD5  = pRule.m_sMD5;

	m_nHashWeight = pRule.m_nHashWeight;

	return *this;
}

//bool CHashRule::operator==(const CHashRule& pRule)
//{
//	return
//		( m_oSHA1 == pRule.m_oSHA1 ) &&
//		( m_oED2K == pRule.m_oED2K ) &&
//		( m_oBTIH == pRule.m_oBTIH ) &&
//		( m_oTTH  == pRule.m_oTTH  ) &&
//		( m_oMD5  == pRule.m_oMD5  );
//}
//bool CHashRule::operator!=(const CHashRule& pRule)
//{
//	return !( *this == pRule );
//}

/*bool CHashRule::Match(const CShareazaFile* pFile) const
{
#ifdef _DEBUG
	ASSERT( pFile && m_nType == srContentHash );
#endif //_DEBUG

	if ( !pFile )
		return false;

	return Match( pFile->m_oSHA1  ? pFile->m_oSHA1.toUrn()  : _T(""),
				  pFile->m_oED2K  ? pFile->m_oED2K.toUrn()  : _T(""),
				  pFile->m_oTiger ? pFile->m_oTiger.toUrn() : _T(""),
				  pFile->m_oBTH   ? pFile->m_oBTH.toUrn()   : _T(""),
				  pFile->m_oMD5   ? pFile->m_oMD5.toUrn()   : _T("") );
}*/
bool CHashRule::Match(const QString& sSHA1,
							const QString& sED2K,
							const QString& sTTH ,
							const QString& sBTIH,
							const QString& sMD5  ) const
{
#ifdef _DEBUG
	ASSERT( m_nType == srContentHash );
#endif //_DEBUG

	bool result =
		( sSHA1.isEmpty() || m_sSHA1.isEmpty() || !( m_sSHA1.compare( sSHA1 ) ) ) &&
		( sED2K.isEmpty() || m_sED2K.isEmpty() || !( m_sED2K.compare( sED2K ) ) ) &&
		( sBTIH.isEmpty() || m_sBTIH.isEmpty() || !( m_sBTIH.compare( sBTIH ) ) ) &&
		( sTTH.isEmpty()  || m_sTTH.isEmpty()  || !( m_sTTH.compare ( sTTH  ) ) ) &&
		( sMD5.isEmpty()  || m_sMD5.isEmpty()  || !( m_sMD5.compare ( sMD5  ) ) );

	if( ( sSHA1.isEmpty() || m_sSHA1.isEmpty() ) &&
		( sED2K.isEmpty() || m_sED2K.isEmpty() ) &&
		( sBTIH.isEmpty() || m_sBTIH.isEmpty() ) &&
		( sTTH.isEmpty()  || m_sTTH.isEmpty()  ) &&
		( sMD5.isEmpty()  || m_sMD5.isEmpty()  ) )
	{
		// If there is no pair of hashes we can compare, there is evidently no match.
		return false;
	}
	else
	{
		return result;
	}

	// Other implementation. As I don't know which one is faster, I'll keep it here for review.
	/*BYTE count = 0;

	if( !( m_sSHA1.isEmpty() || sSHA1.isEmpty() ) )
	{
		if( m_sSHA1 != sSHA1 )
		{
			return false;
		}
		count++;
	}
	if( !( m_sED2K.isEmpty() || sED2K.isEmpty() ) )
	{
		if( m_sED2K != sED2K )
		{
			return false;
		}
		count++;
	}
	if( !( m_sBTIH.isEmpty() || sBTIH.isEmpty() ) )
	{
		if( m_sBTIH != sBTIH )
		{
			return false;
		}
		count++;
	}
	if( !( m_sTTH.isEmpty() || sTTH.isEmpty() ) )
	{
		if( m_sTTH != sTTH )
		{
			return false;
		}
		count++;
	}
	if( !( m_sMD5.isEmpty() || sMD5.isEmpty() ) )
	{
		if( m_sMD5 != sMD5 )
		{
			return false;
		}
		count++;
	}

	// at least one pair of hashes needs to match for us to return true.
	if( count )
	{
		return true;
	}
	else
	{
		return false;
	}*/
}

void CHashRule::SetContentWords(const QString& strContent)
{
#ifdef _DEBUG
	ASSERT( m_nType == srContentHash );
#endif //_DEBUG

	QString tmp;
	int pos1, pos2;

	m_nHashWeight = 1;

	bool bSHA1_set = false,
		 bED2K_set = false,
		 bBTIH_set = false,
		 bTTH_set  = false,
		 bMD5_set  = false,
		 bSHA1_err = false,
		 bED2K_err = false,
		 bBTIH_err = false,
		 bTTH_err  = false,
		 bMD5_err  = false;

	pos1 = strContent.indexOf( "urn:sha1:" );
	if( pos1 != -1 )
	{
		tmp  = strContent.mid( pos1 );
		pos2 = tmp.indexOf( ' ' );
		if( pos2 == 32 + 9 )
		{
			m_sSHA1 = tmp.left( pos2 );
			m_nHashWeight += 11;
			bSHA1_set = true;
		}
		else if( pos2 == -1 && tmp.length() == 32 + 9 )
		{
			m_sSHA1 = tmp;
			m_nHashWeight *= 11;
			bSHA1_set = true;
		}
		else
		{
			bSHA1_err = true;
		}
	}

	pos1 = strContent.indexOf( "urn:ed2k:" );
	if( pos1 != -1 )
	{
		tmp  = strContent.mid( pos1 );
		pos2 = tmp.indexOf( ' ' );
		if( pos2 == 32 + 9 )
		{
			m_sED2K = tmp.left( pos2 );
			m_nHashWeight *= 7;
			bED2K_set = true;
		}
		else if( pos2 == -1 && tmp.length() == 32 + 9 )
		{
			m_sED2K = tmp;
			m_nHashWeight *= 7;
			bED2K_set = true;
		}
		else
		{
			bED2K_err = true;
		}
	}

	pos1 = strContent.indexOf( "urn:tth:" ); // is this prefix correct?
	if( pos1 != -1 )
	{
		tmp  = strContent.mid( pos1 );
		pos2 = tmp.indexOf( ' ' );
		if( pos2 == 39 + 8 )
		{
			m_sTTH = tmp.left( pos2 );
			m_nHashWeight *= 5;
			bTTH_set = true;
		}
		else if( pos2 == -1 && tmp.length() == 39 + 8 )
		{
			m_sTTH = tmp;
			m_nHashWeight *= 5;
			bTTH_set = true;
		}
		else
		{
			bTTH_err = true;
		}
	}

	pos1 = strContent.indexOf( "urn:btih:" ); // is this prefix correct?
	if( pos1 != -1 )
	{
		tmp  = strContent.mid( pos1 );
		pos2 = tmp.indexOf( ' ' );
		if( pos2 == 32 + 9 )
		{
			m_sBTIH = tmp.left( pos2 );
			m_nHashWeight *= 3;
			bBTIH_set = true;
		}
		else if( pos2 == -1 && tmp.length() == 32 + 9 )
		{
			m_sBTIH = tmp;
			m_nHashWeight *= 3;
			bBTIH_set = true;
		}
		else
		{
			bBTIH_err = true;
		}
	}

	pos1 = strContent.indexOf( "urn:md5:" );
	if( pos1 != -1 )
	{
		tmp  = strContent.mid( pos1 );
		pos2 = tmp.indexOf( ' ' );
		if( pos2 == 32 + 8 )
		{
			m_sMD5 = tmp.left( pos2 );
			m_nHashWeight *= 2;
			bMD5_set = true;
		}
		else if( pos2 == -1 && tmp.length() == 32 + 8 )
		{
			m_sMD5 = tmp;
			m_nHashWeight *= 2;
			bMD5_set = true;
		}
		else
		{
			bMD5_err = true;
		}
	}

#ifdef _DEBUG
	if( bSHA1_set || bTTH_set || bED2K_set || bBTIH_set || bMD5_set )
	{
		if( bSHA1_err ||bTTH_err || bED2K_err || bBTIH_err || bMD5_err )
			theApp.Message( MSG_ERROR, IDS_SECURITY_XML_HASH_RULE_IMPORT_ERROR );
	}
	else
	{
		theApp.Message( MSG_ERROR, IDS_SECURITY_XML_HASH_RULE_IMPORT_FAIL );
	}
#endif //_DEBUG
}
QString CHashRule::GetContentWords() const
{
#ifdef _DEBUG
	ASSERT( m_nType == srContentHash );
#endif //_DEBUG

	QString strWords = "";

	if( m_sSHA1.length() )
	{
		strWords += m_sSHA1;
		strWords += " ";
	}

	if( m_sED2K.length() )
	{
		strWords += m_sED2K;
		strWords += " ";
	}

	if( m_sBTIH.length() )
	{
		strWords += m_sBTIH;
		strWords += " ";
	}

	if( m_sTTH.length() )
	{
		strWords += m_sTTH;
		strWords += " ";
	}

	if( m_sMD5.length() )
	{
		strWords += m_sMD5;
		strWords += " ";
	}

	strWords.trimmed();

	return strWords;
}
//////////////////////////////////////////////////////////////////////
// CRegExpRule

CRegExpRule::CRegExpRule(bool)
{
	m_sContent = "";
	m_nType = srContentRegExp;
}

CRegExpRule& CRegExpRule::operator=(const CRegExpRule& pRule)
{
#ifdef _DEBUG
	ASSERT( &pRule && pRule.m_nType == srContentRegExp );
#endif //_DEBUG

	this->CSecureRule::operator=( pRule );

	m_sContent = pRule.m_sContent;

	return *this;
}

// I suppose this is correct... :) Nothing has been changed.
/*bool CRegExpRule::Match(const CQuerySearch* pQuery, const QString& strContent) const
{
#ifdef _DEBUG
	ASSERT( pQuery && m_nType == srContentRegExp );
#endif //_DEBUG

	if ( !pQuery || m_sContent.isEmpty() )
		return false;

	// Build a regular expression filter from the search query words.
	// Returns an empty string if not applied or if the filter was invalid.
	//
	// Substitutes:
	// <_> - inserts all query keywords;
	// <1>..<9> - inserts query keyword number 1..9;
	// <> - inserts next query keyword.
	//
	// For example regular expression:
	//	.*(<2><1>)|(<_>).*
	// for "music mp3" query will be converted to:
	//	.*(mp3\s*music\s*)|(music\s*mp3\s*).*
	//
	// Note: \s* - matches any number of white-space symbols (including zero).

	QString strFilter;
	int nTotal = 0;
	for ( LPCTSTR pszPattern = m_sContent; *pszPattern; ++pszPattern )
	{
		LPCTSTR pszLt = _tcschr( pszPattern, _T('<') );
		if ( pszLt )
		{
			int nLength = pszLt - pszPattern;
			if ( nLength )
			{
				strFilter.Append( pszPattern, nLength );
			}

			pszPattern = pszLt + 1;
			bool bEnds = false;
			bool bAll = ( *pszPattern == '_' );
			for ( ; *pszPattern ; pszPattern++ )
			{
				if ( *pszPattern == '>' )
				{
					bEnds = true;
					break;
				}
			}
			if ( bEnds )
			{
				if ( bAll )
				{
					// Add all keywords at the "<_>" position
					for ( CQuerySearch::const_iterator i = pQuery->begin();
						i != pQuery->end(); ++i )
					{
						strFilter.AppendFormat( L"%s\\s*",
							QString( i->first, (int)( i->second ) ) );
					}
				}
				else
				{
					pszPattern--; // Go back
					int nNumber = 0;

					// Numbers from 1 to 9, no more
					if ( _stscanf( &pszPattern[0], L"%i", &nNumber ) != 1 )
						nNumber = ++nTotal;

					int nWord = 1;
					for ( CQuerySearch::const_iterator i = pQuery->begin();
						i != pQuery->end(); ++i, ++nWord )
					{
						if ( nWord == nNumber )
						{
							strFilter.AppendFormat( L"%s\\s*",
								QString( i->first, (int)( i->second ) ) );
							break;
						}
					}
					pszPattern++; // return to the last position
				}
			}
			else
			{
				// no closing '>'
				strFilter.Empty();
				break;
			}
		}
		else
		{
			strFilter += pszPattern;
			break;
		}
	}

	if ( strFilter.isEmpty() )
		return false;

	return ( RegExp::Match( strFilter, strContent ) != 0 ); // "!= 0" disables compiler warning.
}*/

void CRegExpRule::SetContentWords(const QString& strContent)
{
#ifdef _DEBUG
	ASSERT( m_nType == srContentRegExp );
#endif //_DEBUG

	m_sContent = strContent;
}
QString CRegExpRule::GetContentWords() const
{
#ifdef _DEBUG
	ASSERT( m_nType == srContentRegExp );
#endif //_DEBUG

	return m_sContent;
}

//////////////////////////////////////////////////////////////////////
// CUserAgentRule

CUserAgentRule::CUserAgentRule(bool)
{
	m_nType = srContentUserAgent;

	m_sContent = "";
	m_bRegExp  = false;
}

CUserAgentRule& CUserAgentRule::operator=(const CUserAgentRule& pRule)
{
#ifdef _DEBUG
	ASSERT( &pRule && pRule.m_nType == srContentUserAgent );
#endif //_DEBUG

	this->CSecureRule::operator=( pRule );

	m_bRegExp  = pRule.m_bRegExp;
	m_sContent = pRule.m_sContent;

	return *this;
}

bool CUserAgentRule::Match(const QString& strUserAgent) const
{
#ifdef _DEBUG
	ASSERT( m_nType == srContentUserAgent );
#endif //_DEBUG

	if( m_bRegExp )
	{
//		return ( RegExp::Match( m_sContent, strUserAgent ) != 0 ); // "!= 0" disables compiler warning.
	}
	else
	{
		return ( strUserAgent.indexOf( m_sContent ) != -1 );
	}

	return true;
}

void CUserAgentRule::SetContentWords(const QString& strContent)
{
#ifdef _DEBUG
	ASSERT( m_nType == srContentUserAgent );
#endif //_DEBUG

	m_sContent = strContent;
	m_sContent.trimmed();
}
QString CUserAgentRule::GetContentWords() const
{
#ifdef _DEBUG
	ASSERT( m_nType == srContentUserAgent );
#endif //_DEBUG

	return m_sContent;
}

//////////////////////////////////////////////////////////////////////
// CContentRule

CContentRule::CContentRule(bool)
{
	m_nType = srContentAll;
}

CContentRule& CContentRule::operator=(const CContentRule& pRule)
{
#ifdef _DEBUG
	ASSERT( &pRule && ( pRule.m_nType == srContentAny || pRule.m_nType == srContentAll ) );
#endif //_DEBUG

	this->CSecureRule::operator=( pRule );

	m_lContent = pRule.m_lContent;

	return *this;
}

bool CContentRule::Match(const QString& strContent) const
{
#ifdef _DEBUG
	ASSERT( m_nType == srContentAny || m_nType == srContentAll );
#endif //_DEBUG

	for ( CListIterator i = m_lContent.begin() ; i != m_lContent.end() ; i++ )
	{
		bool bFound = strContent.indexOf( *i ) != -1;

		if ( bFound && m_nType == srContentAny )
		{
			return true;
		}
		else if ( !bFound && m_nType == srContentAll )
		{
			return false;
		}
	}

	if ( m_nType == srContentAll )
		return true;

	return false;
}
// TODO: use method to safely find out extension instead of only looking for last dot in filename
/*bool CContentRule::Match(const CShareazaFile* pFile) const
{
#ifdef _DEBUG
	ASSERT( pFile && ( m_nType == srContentAny || m_nType == srContentAll ) );
#endif //_DEBUG

	if ( pFile )
	{
		if ( pFile->m_nSize != 0 && pFile->m_nSize != SIZE_UNKNOWN )
		{
			if ( m_sName.lastIndexOf( '.' ) != -1 )
			{
				QString strExt = pFile->m_sName.mid( m_sName.lastIndexOf( '.' ) );
				QString strExtFileSize = "size:%1:%2";
				strExtFileSize.arg( strExt, QString( pFile->m_nSize ) );
				if ( Match( strExtFileSize ) )
					return true;
			}
		}

		if ( Match( pFile->m_sName ) )
			return true;
	}
	return false;
}*/

void CContentRule::SetContentWords(const QString& strContent)
{
#ifdef _DEBUG
	ASSERT( m_nType == srContentAny || m_nType == srContentAll );
#endif //_DEBUG

	QString sContent = strContent;
	sContent.replace( '\t', ' ' );

	m_lContent.clear();

	QString tmp;

	while ( !sContent.isEmpty() )
	{
		sContent.trimmed();
		int index = sContent.indexOf( ' ' );
		tmp = ( index != -1 ) ? sContent.left( index ) : sContent;
		if ( !tmp.isEmpty() )
			m_lContent.push_back( tmp );
		sContent = sContent.mid( ( index != -1 ) ? index : 0 );
	}
}
QString CContentRule::GetContentWords() const
{
#ifdef _DEBUG
	ASSERT( m_nType == srContentAny || m_nType == srContentAll );
#endif //_DEBUG

	QString res;
	for ( CListIterator i = m_lContent.begin() ; i != m_lContent.end() ; i++ )
	{
		res = res + *i;
	}

	return res;
}

void CContentRule::SetAll(bool all)
{
	m_nType = all ? srContentAll : srContentAny;
}

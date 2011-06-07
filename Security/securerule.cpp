#include "security.h"
#include "securerule.h"

using namespace security;

CSecureRule::CSecureRule(bool bCreate)
{
	m_nType		= srContentUndefined; // invalidates rule as long as it does not contain any useful content.
	m_nAction	= srDeny;
	m_sComment	= "";
	m_tExpire	= srIndefinite;
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
	m_tExpire	= pRule.m_tExpire;
	m_nToday	= pRule.m_nToday;
	m_nTotal	= pRule.m_nTotal;

	return *this;
}

//////////////////////////////////////////////////////////////////////
// CSecureRule expiry check

bool CSecureRule::isExpired(quint32 tNow, bool bSession) const
{
	if ( m_tExpire == srIndefinite ) return false;
	if ( m_tExpire == srSession ) return bSession;
	return m_tExpire < tNow;
}

CSecureRule* CSecureRule::getCopy() const
{
	// This method should never be called.
#ifdef _DEBUG
	Q_ASSERT( false );
#endif //_DEBUG

	return new CSecureRule( *this );
}

//////////////////////////////////////////////////////////////////////
// CSecureRule match

bool CSecureRule::match(const QHostAddress&) const
{
	return false;
}
bool CSecureRule::match(const QString&) const
{
	return false;
}
/*bool CSecureRule::match(const CShareazaFile*) const
{
	return false;
}
bool CSecureRule::match(const CQuerySearch*, const QString&) const
{
	return false;
}*/

//////////////////////////////////////////////////////////////////////
// CSecureRule serialize

void CSecureRule::save(const CSecureRule* pRule, QDataStream &oStream)
{
	oStream << (quint8)(pRule->m_nType);
	oStream << (quint8)(pRule->m_nAction);
	oStream << pRule->m_sComment;
	oStream << pRule->m_oUUID.toString();
	oStream << pRule->m_tExpire;
	oStream << pRule->m_nTotal;

	switch ( pRule->m_nType )
	{
	case srContentAddress:
		oStream << ((CIPRule*)pRule)->getIP().toString();
		break;
	case srContentAddressRange:
		oStream << ((CIPRangeRule*)pRule)->getIP().toString();
		oStream << ((CIPRangeRule*)pRule)->getMask();
		break;
	case srContentCountry:
		oStream << ((CCountryRule*)pRule)->getContent();
		break;
	case srContentHash:
		oStream << ((CHashRule*)pRule)->getContentWords();
		break;
	case srContentAny:
	case srContentAll:
		oStream << ((CContentRule*)pRule)->getContentWords();
		oStream << ( ( pRule->getType() == srContentAll ) ? true : false );
		break;
	case srContentRegExp:
		oStream << ((CRegExpRule*)pRule)->getContentWords();
		break;
	case srContentUserAgent:
		oStream << ((CUserAgentRule*)pRule)->getContentWords();
		break;
	default:
		// Do nothing. We don't store invalid rules.
		break;
	}
}

void CSecureRule::load(CSecureRule* pRule, QDataStream &oStream, int)
{
	if( pRule )
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

	oStream >> nType;
	oStream >> nAction;
	oStream >> sComment;
	oStream >> sUUID;
	oStream >> tExpire;
	oStream >> nTotal;

	QString sTmp;
	bool	bAll = true;

	switch ( nType )
	{
	case 0:
		// contentless rule
		pRule = new CSecureRule();
		break;
	case 1:
		pRule = new CIPRule();
		oStream >> sTmp;
		{
			QHostAddress oTmp( sTmp );
			((CIPRule*)pRule)->setIP( oTmp );
		}
		break;
	case 2:
		pRule = new CIPRangeRule();
		oStream >> sTmp;
		{
			QHostAddress oTmp( sTmp );
			((CIPRangeRule*)pRule)->setIP( oTmp );
			quint32 nTmp;
			oStream >> nTmp;
			((CIPRangeRule*)pRule)->setMask( nTmp );
		}
		break;
	case 3:
		pRule = new CCountryRule();
		oStream >> sTmp;
		((CCountryRule*)pRule)->setContent( sTmp );
		break;
	case 4:
		pRule = new CHashRule();
		oStream >> sTmp;
		((CHashRule*)pRule)->setContentWords( sTmp );
		break;
	case 5:
	case 6:
		pRule = new CContentRule();
		oStream >> sTmp;
		oStream >> bAll;
		((CContentRule*)pRule)->setContentWords( sTmp );
		((CContentRule*)pRule)->setAll( bAll );
		break;
	case 7:
		pRule = new CRegExpRule();
		oStream >> sTmp;
		((CRegExpRule*)pRule)->setContentWords( sTmp );
		break;
	case 8:
		pRule = new CUserAgentRule();
		oStream >> sTmp;
		((CUserAgentRule*)pRule)->setContentWords( sTmp );
		break;
	default:
		// There is an empty or erroneous rule. Error handling (if necessary) should go here.
//		theApp.Message( MSG_ERROR, IDS_SECURITY_ARCHIVE_RULE_LOAD_FAIL );
		break;
	}

	pRule->m_nAction  = (Policy)nAction;
	pRule->m_sComment = sComment;
	pRule->m_oUUID    = QUuid( sUUID );
	pRule->m_tExpire  = tExpire;
	pRule->m_nTotal   = nTotal;
}

//////////////////////////////////////////////////////////////////////
// CSecureRule XML

CSecureRule* CSecureRule::fromXML(const QDomElement &oXMLelement)
{
	QString strType = oXMLelement.attribute( "type" );

	if ( strType.isEmpty() )
		return NULL;

	CSecureRule* pRule = NULL;

	if ( strType.compare( "address", Qt::CaseInsensitive ) == 0 )
	{
		QString sAddress = oXMLelement.attribute( "address" );
		QString sMask    = oXMLelement.attribute( "mask" );

		QHostAddress oIP;
		if ( oIP.setAddress( sAddress ) )
		{
			if ( oIP.protocol() == QAbstractSocket::IPv4Protocol )
			{
				quint8 x[4];
				qint8 pos;
				bool maskSet = false;

				QString sByte;
				// This converts the string smask into an array of bytes (quint8).
				for ( quint8 i = 0; i < 4; i++ )
				{
					pos = sMask.indexOf( '.' );

					if ( sMask.isEmpty() || pos == -1 && ( i != 3 || sMask.length() > 3 ) || !pos || pos > 2 )
					{
						break;
					}

					if ( pos != -1 )
					{
						sByte = sMask.left( pos );
						sMask = sMask.mid( pos + 1 );
					}
					else
					{
						sByte = sMask;
						sMask.clear();
					}

					int nTmp = sByte.toInt();

					if ( nTmp > -1 && nTmp < 256 )
					{
						x[i] = (quint8)nTmp;
					}
					else
					{
						break;
					}

					if ( pos == -1 && *(quint32*)x != 0xFFFFFFFF)
						maskSet = true;
				}

				if ( maskSet )
				{
					CIPRangeRule* rule = new CIPRangeRule();
					rule->setIP( oIP );
					rule->setMask( *(quint32*)x );

					pRule = rule;
				}
				else
				{
					CIPRule* rule = new CIPRule();
					rule->setIP( oIP );

					pRule = rule;
				}
			}

			// TODO: implement dealing with IPv6 subnets

			else // we're dealing with IPv6 - note that this won't work with subnets...
			{
				CIPRule* rule = new CIPRule();
				rule->setIP( oIP );

				pRule = rule;
			}
		}
		else
		{
			// Error in XML entry.
			return NULL;
		}
	}
	else if ( strType.compare( "hash", Qt::CaseInsensitive ) == 0 )
	{
		CHashRule* rule = new CHashRule();
		rule->setContentWords( oXMLelement.attribute( "content" ) );

		if ( rule->getContentWords().trimmed().isEmpty() )
		{
			delete rule;
			return NULL;
		}

		pRule = rule;
	}
	else if ( strType.compare( "regexp", Qt::CaseInsensitive ) == 0 )
	{
		CRegExpRule* rule = new CRegExpRule();
		rule->setContentWords( oXMLelement.attribute( "content" ) );

		if ( rule->getContentWords().trimmed().isEmpty() )
		{
			delete rule;
			return NULL;
		}

		pRule = rule;
	}
	else if ( strType.compare( "content", Qt::CaseInsensitive ) == 0 )
	{
		QString strMatch = oXMLelement.attribute( "match" );
		QString strContent = oXMLelement.attribute( "content" );

		QString strUrn = strContent.left( 4 );

		// This handles "old style" Shareaza RegExp rules.
		if ( strMatch.compare( "regexp", Qt::CaseInsensitive ) == 0 )
		{
			CRegExpRule* rule = new CRegExpRule();
			rule->setContentWords( strContent );

			if ( rule->getContentWords().trimmed().isEmpty() )
			{
				delete rule;
				return NULL;
			}

			pRule = rule;
		}
		// This handles "old style" Shareaza hash rules.
		else if ( strUrn.compare( "urn:", Qt::CaseInsensitive ) == 0 )
		{
			CHashRule* rule = new CHashRule();
			rule->setContentWords( oXMLelement.attribute( "content" ) );

			if ( rule->getContentWords().trimmed().isEmpty() )
			{
				delete rule;
				return NULL;
			}

			pRule = rule;
		}
		else
		{
			bool all = ( strMatch.compare( "all", Qt::CaseInsensitive ) == 0 );

			if ( all || strMatch.compare( "any", Qt::CaseInsensitive ) == 0 )
			{
				CContentRule* rule = new CContentRule();
				rule->setAll( all );
				rule->setContentWords( oXMLelement.attribute( "content" ) );

				if ( rule->getContentWords().trimmed().isEmpty() )
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
	else if ( strType.compare( "country", Qt::CaseInsensitive ) == 0 )
	{
		CCountryRule* rule = new CCountryRule();
		rule->setContent	( oXMLelement.attribute( "content" ) );

		if ( rule->getContent().trimmed().isEmpty() )
		{
			delete rule;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}

	QString strAction = oXMLelement.attribute( "action" );

	if ( strAction.compare( "null", Qt::CaseInsensitive ) == 0 )
	{
		pRule->m_nAction = srNull;
	}
	else if ( strAction.compare( "accept", Qt::CaseInsensitive ) == 0 )
	{
		pRule->m_nAction = srAccept;
	}
	else if ( strAction.compare( "deny", Qt::CaseInsensitive ) == 0 || strAction.isEmpty() )
	{
		pRule->m_nAction = srDeny;
	}
	else
	{
		delete pRule;
		return NULL;
	}

	pRule->m_sComment = oXMLelement.attribute( "comment" ).trimmed();

	QString strExpire = oXMLelement.attribute( "expire" ).trimmed();
	if ( strExpire.compare( "session", Qt::CaseInsensitive ) == 0 )
	{
		pRule->m_tExpire = srSession;
	}
	else if ( strExpire.compare( "indefinite", Qt::CaseInsensitive ) == 0 )
	{
		pRule->m_tExpire = srIndefinite;
	}
	else
	{
		pRule->m_tExpire = strExpire.toInt();
	}

	QString strUUID = oXMLelement.attribute( "uuid" ).trimmed();
	if ( strUUID.isEmpty() )
	{
		strUUID = oXMLelement.attribute( "guid" ).trimmed();
	}
	if ( strUUID.isEmpty() )
	{
		pRule->m_oUUID = QUuid::createUuid();
	}
	else
	{
		pRule->m_oUUID = QUuid( strUUID );
	}

	return pRule;
}

void CSecureRule::toXML( QDomElement& ) const
{
	// As this method is virtual and the main class should never be used directly,
	// we got a problem if this method is ever called.
	Q_ASSERT( false );
}

// Contains default code for XML generation.
void CSecureRule::toXML(const CSecureRule& oRule, QDomElement& oXMLelement)
{
	if ( !( oRule.m_sComment.isEmpty() ) )
	{
		oXMLelement.setAttribute( "comment", oRule.m_sComment );
	}

	QString sValue;
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
	oXMLelement.setAttribute( "action", sValue );

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
	oXMLelement.setAttribute( "expire", sValue );

	sValue = oRule.m_oUUID.toString();
	oXMLelement.setAttribute( "uuid", sValue );
}

//////////////////////////////////////////////////////////////////////
// CIPRule

CIPRule::CIPRule(bool)
{
	m_nType = srContentAddress;
}

CIPRule& CIPRule::operator=(const CIPRule& pRule)
{
#ifdef _DEBUG
	Q_ASSERT( &pRule && pRule.m_nType == srContentAddress );
#endif //_DEBUG

	this->CSecureRule::operator=( pRule );

	m_oIP = pRule.m_oIP;

	return *this;
}

bool CIPRule::match(const QHostAddress& oAddress) const
{
#ifdef _DEBUG
	Q_ASSERT( !oAddress.isNull() && m_nType == srContentAddress );
#endif //_DEBUG

	if ( !oAddress.isNull() && oAddress == m_oIP )
	{
		return true;
	}
	return false;
}

void CIPRule::toXML( QDomElement& oXMLroot ) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentAddress );
#endif //_DEBUG

	QDomElement oElement = oXMLroot.ownerDocument().createElement( "rule" );

	oElement.setAttribute( "type", "address" );
	oElement.setAttribute( "address", m_oIP.toString() );

	CSecureRule::toXML( *this, oElement );

	oXMLroot.appendChild( oElement );
}

//////////////////////////////////////////////////////////////////////
// CIPRangeRule

CIPRangeRule::CIPRangeRule(bool)
{
	m_nType = srContentAddressRange;
}

CIPRangeRule& CIPRangeRule::operator=(const CIPRangeRule& pRule)
{
#ifdef _DEBUG
	Q_ASSERT( &pRule && pRule.m_nType == srContentAddressRange );
#endif //_DEBUG

	this->CSecureRule::operator=( pRule );

	m_oIP   = pRule.m_oIP;
	m_oMask = pRule.m_oMask;

	return *this;
}



// todo: continue here.



// todo: make this work with IPv6.
bool CIPRangeRule::match(const QHostAddress &oAddress) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentAddressRange );
#endif //_DEBUG

	if ( oAddress.protocol() == QAbstractSocket::IPv6Protocol )
	{
		return oAddress.isInSubnet( m_oIP, 0 );
	}
	else
	{
		return oAddress.isInSubnet( m_oIP, m_oMask.toIPv4Address() );
	}
}

void CIPRangeRule::toXML( QDomElement& oXMLroot ) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentAddressRange );
#endif //_DEBUG

	QDomElement oElement = oXMLroot.ownerDocument().createElement( "rule" );

	oElement.setAttribute( "type", "address" );
	oElement.setAttribute( "mask", m_oMask.toString() );
	oElement.setAttribute( "address", m_oIP.toString() );

	CSecureRule::toXML( *this, oElement );

	oXMLroot.appendChild( oElement );
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
	Q_ASSERT( &pRule && pRule.m_nType == srContentCountry );
#endif //_DEBUG

	this->CSecureRule::operator=( pRule );

	m_sCountry = pRule.m_sCountry;

	return *this;
}

bool CCountryRule::match(const QHostAddress& oAddress) const
{
#ifdef _DEBUG
	Q_ASSERT( !oAddress.isNull() && m_nType == srContentCountry );
#endif //_DEBUG

	if ( !oAddress.isNull() /*&& theApp.getCountryCode( *pAddress ) == m_sCountry*/ )
		return true;

	return false;
}

void CCountryRule::toXML( QDomElement& oXMLroot ) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentCountry );
#endif //_DEBUG

	QDomElement oElement = oXMLroot.ownerDocument().createElement( "rule" );

	oElement.setAttribute( "type", "country" );
	oElement.setAttribute( "content", m_sCountry );

	CSecureRule::toXML( this, oElement );

	oXMLroot.appendChild( oElement );
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
	Q_ASSERT( &pRule && pRule.m_nType == srContentHash );
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

/*bool CHashRule::match(const CShareazaFile* pFile) const
{
#ifdef _DEBUG
	Q_ASSERT( pFile && m_nType == srContentHash );
#endif //_DEBUG

	if ( !pFile )
		return false;

	return match( pFile->m_oSHA1  ? pFile->m_oSHA1.toUrn()  : _T(""),
				  pFile->m_oED2K  ? pFile->m_oED2K.toUrn()  : _T(""),
				  pFile->m_oTiger ? pFile->m_oTiger.toUrn() : _T(""),
				  pFile->m_oBTH   ? pFile->m_oBTH.toUrn()   : _T(""),
				  pFile->m_oMD5   ? pFile->m_oMD5.toUrn()   : _T("") );
}*/
bool CHashRule::match(const QString& sSHA1,
							const QString& sED2K,
							const QString& sTTH ,
							const QString& sBTIH,
							const QString& sMD5  ) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentHash );
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

void CHashRule::setContentWords(const QString& strContent)
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentHash );
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
//		if( bSHA1_err ||bTTH_err || bED2K_err || bBTIH_err || bMD5_err )
//			theApp.Message( MSG_ERROR, IDS_SECURITY_XML_HASH_RULE_IMPORT_ERROR );
	}
	else
	{
//		theApp.Message( MSG_ERROR, IDS_SECURITY_XML_HASH_RULE_IMPORT_FAIL );
	}
#endif //_DEBUG
}
QString CHashRule::getContentWords() const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentHash );
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

void CHashRule::toXML( QDomElement& oXMLroot ) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentHash );
#endif //_DEBUG

	QDomElement oElement = oXMLroot.ownerDocument().createElement( "rule" );

	oElement.setAttribute( "type", "hash" );
	oElement.setAttribute( "content", getContentWords() );

	CSecureRule::toXML( this, oElement );

	oXMLroot.appendChild( oElement );
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
	Q_ASSERT( &pRule && pRule.m_nType == srContentRegExp );
#endif //_DEBUG

	this->CSecureRule::operator=( pRule );

	m_sContent = pRule.m_sContent;

	return *this;
}

// I suppose this is correct... :) Nothing has been changed.
/*bool CRegExpRule::match(const CQuerySearch* pQuery, const QString& strContent) const
{
#ifdef _DEBUG
	Q_ASSERT( pQuery && m_nType == srContentRegExp );
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

	return ( RegExp::match( strFilter, strContent ) != 0 ); // "!= 0" disables compiler warning.
}*/

void CRegExpRule::setContentWords(const QString& strContent)
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentRegExp );
#endif //_DEBUG

	m_sContent = strContent;
}
QString CRegExpRule::getContentWords() const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentRegExp );
#endif //_DEBUG

	return m_sContent;
}

void CRegExpRule::toXML( QDomElement& oXMLroot ) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentRegExp );
#endif //_DEBUG

	QDomElement oElement = oXMLroot.ownerDocument().createElement( "rule" );

	oElement.setAttribute( "type", "regexp" );
	oElement.setAttribute( "content", getContentWords() );

	CSecureRule::toXML( this, oElement );

	oXMLroot.appendChild( oElement );
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
	Q_ASSERT( &pRule && pRule.m_nType == srContentUserAgent );
#endif //_DEBUG

	this->CSecureRule::operator=( pRule );

	m_bRegExp  = pRule.m_bRegExp;
	m_sContent = pRule.m_sContent;

	return *this;
}

bool CUserAgentRule::match(const QString& strUserAgent) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentUserAgent );
#endif //_DEBUG

	if( m_bRegExp )
	{
//		return ( RegExp::match( m_sContent, strUserAgent ) != 0 ); // "!= 0" disables compiler warning.
	}
	else
	{
		return ( strUserAgent.indexOf( m_sContent ) != -1 );
	}

	return true;
}

void CUserAgentRule::setContentWords(const QString& strContent)
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentUserAgent );
#endif //_DEBUG

	m_sContent = strContent;
	m_sContent.trimmed();
}
QString CUserAgentRule::getContentWords() const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentUserAgent );
#endif //_DEBUG

	return m_sContent;
}

void CUserAgentRule::toXML( QDomElement& oXMLroot ) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentUserAgent );
#endif //_DEBUG

	QDomElement oElement = oXMLroot.ownerDocument().createElement( "rule" );

	oElement.setAttribute( "type", "agent" );

	if( m_bRegExp )
	{
		oElement.setAttribute( "match", "regexp" );
	}
	else
	{
		oElement.setAttribute( "match", "list" );
	}

	oElement.setAttribute( "content", getContentWords() );

	CSecureRule::toXML( this, oElement );

	oXMLroot.appendChild( oElement );
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
	Q_ASSERT( &pRule && ( pRule.m_nType == srContentAny || pRule.m_nType == srContentAll ) );
#endif //_DEBUG

	this->CSecureRule::operator=( pRule );

	m_lContent = pRule.m_lContent;

	return *this;
}

bool CContentRule::match(const QString& strContent) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentAny || m_nType == srContentAll );
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
/*bool CContentRule::match(const CShareazaFile* pFile) const
{
#ifdef _DEBUG
	Q_ASSERT( pFile && ( m_nType == srContentAny || m_nType == srContentAll ) );
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
				if ( match( strExtFileSize ) )
					return true;
			}
		}

		if ( match( pFile->m_sName ) )
			return true;
	}
	return false;
}*/

void CContentRule::setContentWords(const QString& strContent)
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentAny || m_nType == srContentAll );
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
QString CContentRule::getContentWords() const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentAny || m_nType == srContentAll );
#endif //_DEBUG

	QString res;
	for ( CListIterator i = m_lContent.begin() ; i != m_lContent.end() ; i++ )
	{
		res = res + *i;
	}

	return res;
}

void CContentRule::setAll(bool all)
{
	m_nType = all ? srContentAll : srContentAny;
}

void CContentRule::toXML( QDomElement& oXMLroot ) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentAll || m_nType == srContentAny );
#endif //_DEBUG

	QDomElement oElement = oXMLroot.ownerDocument().createElement( "rule" );

	oElement.setAttribute( "type", "content" );
	oElement.setAttribute( "content", getContentWords() );

	if( m_nType == srContentAll )
	{
		oElement.setAttribute( "match", "all" );
	}
	else
	{
		oElement.setAttribute( "match", "any" );
	}

	CSecureRule::toXML( this, oElement );

	oXMLroot.appendChild( oElement );
}

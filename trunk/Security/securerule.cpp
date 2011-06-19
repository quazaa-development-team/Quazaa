#include "security.h"
#include "securerule.h"
#include "geoiplist.h"

using namespace security;

CSecureRule::CSecureRule(bool bCreate)
{
	m_nType		= srContentUndefined; // invalidates rule as long as it does not contain any useful content.
	m_nAction	= srDeny;
	m_tExpire	= srIndefinite;
	m_nToday	= 0;
	m_nTotal	= 0;

	if ( bCreate )
		m_oUUID = QUuid::createUuid();
}

bool CSecureRule::operator==(const CSecureRule& pRule)
{
	return ( m_nType	== pRule.m_nType	&&
			 m_nAction	== pRule.m_nAction	&&
			 m_sComment	== pRule.m_sComment	&&
			 m_oUUID	== pRule.m_oUUID	&&
			 m_tExpire	== pRule.m_tExpire	&&
			 m_nToday	== pRule.m_nToday	&&
			 m_nTotal	== pRule.m_nTotal   &&
			 m_sContent	== pRule.m_sContent);
}

CSecureRule* CSecureRule::getCopy() const
{
	// This method should never be called.
#ifdef _DEBUG
	Q_ASSERT( false );
#endif //_DEBUG

	return new CSecureRule( *this );
}

QString CSecureRule::getContentString() const
{
	// This method should never be called.
#ifdef _DEBUG
	Q_ASSERT( m_nType != srContentUndefined );
#endif //_DEBUG

	return m_sContent;
}

bool CSecureRule::isExpired(quint32 tNow, bool bSession) const
{
	if ( m_tExpire == srIndefinite ) return false;
	if ( m_tExpire == srSession ) return bSession;
	return m_tExpire < tNow;
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
bool CSecureRule::match(const CFile&) const
{
 return false;
}
/*bool CSecureRule::match(const CQuerySearch*, const QString&) const
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
		oStream << ((CCountryRule*)pRule)->getContentString();
		break;
	case srContentHash:
		oStream << ((CHashRule*)pRule)->getContentString();
		break;
	case srContentAny:
	case srContentAll:
		oStream << ((CContentRule*)pRule)->getContentString();
		oStream << ( ( pRule->getType() == srContentAll ) ? true : false );
		break;
	case srContentRegExp:
		oStream << ((CRegExpRule*)pRule)->getContentString();
		break;
	case srContentUserAgent:
		oStream << ((CUserAgentRule*)pRule)->getContentString();
		oStream << ((CUserAgentRule*)pRule)->getRegExp();
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
		((CCountryRule*)pRule)->setContentString( sTmp );
		break;
	case 4:
		pRule = new CHashRule();
		oStream >> sTmp;
		((CHashRule*)pRule)->setContentString( sTmp );
		break;
	case 5:
	case 6:
		pRule = new CContentRule();
		oStream >> sTmp;
		oStream >> bAll;
		((CContentRule*)pRule)->setContentString( sTmp );
		((CContentRule*)pRule)->setAll( bAll );
		break;
	case 7:
		pRule = new CRegExpRule();
		oStream >> sTmp;
		((CRegExpRule*)pRule)->setContentString( sTmp );
		break;
	case 8:
		pRule = new CUserAgentRule();
		oStream >> sTmp;
		oStream >> bAll;
		((CUserAgentRule*)pRule)->setContentString( sTmp );
		((CUserAgentRule*)pRule)->setRegExp( bAll );
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
		rule->setContentString( oXMLelement.attribute( "content" ) );

		if ( rule->getContentString().trimmed().isEmpty() )
		{
			delete rule;
			return NULL;
		}

		pRule = rule;
	}
	else if ( strType.compare( "regexp", Qt::CaseInsensitive ) == 0 )
	{
		CRegExpRule* rule = new CRegExpRule();
		rule->setContentString( oXMLelement.attribute( "content" ) );

		if ( rule->getContentString().trimmed().isEmpty() )
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
			rule->setContentString( strContent );

			if ( rule->getContentString().trimmed().isEmpty() )
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
			rule->setContentString( oXMLelement.attribute( "content" ) );

			if ( rule->getContentString().trimmed().isEmpty() )
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
				rule->setContentString( oXMLelement.attribute( "content" ) );

				if ( rule->getContentString().trimmed().isEmpty() )
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
		rule->setContentString( oXMLelement.attribute( "content" ) );

		if ( rule->getContentString().trimmed().isEmpty() )
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
	m_nType = srContentCountry;
}

bool CCountryRule::match(const QHostAddress& oAddress) const
{
#ifdef _DEBUG
	Q_ASSERT( !oAddress.isNull() && m_nType == srContentCountry );
#endif //_DEBUG

	if ( !oAddress.isNull() && m_sContent == GeoIP.findCountryCode( oAddress ) )
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
	oElement.setAttribute( "content", m_sContent );

	CSecureRule::toXML( this, oElement );

	oXMLroot.appendChild( oElement );
}


//////////////////////////////////////////////////////////////////////
// CHashRule

CHashRule::CHashRule(bool)
{
	m_nType = srContentHash;
}

/*bool CHashRule::operator!=(const CHashRule& pRule)
{
	return !( *this == pRule );
}*/

QList< CHash > CHashRule::getHashes() const
{
	QList< CHash > result;
	foreach ( CHash oHash, m_Hashes )
	{
		result.append( oHash );
	}

	return result;
}

void CHashRule::setContent(const QList< CHash >& hashes)
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentHash );
#endif //_DEBUG

	m_sContent = "";

	foreach ( CHash oHash, hashes )
	{
		m_Hashes.insert( oHash.getAlgorithm(), oHash );
		m_sContent += oHash.ToURN();
		m_sContent += " ";
	}

	m_sContent = m_sContent.trimmed();
}

void CHashRule::setContentString(const QString& sContent)
{
	QString tmp, sHash;
	int pos1, pos2;
	QList< CHash > lHashes;

	QString prefixes[5] = { "urn:sha1:", "urn:ed2k:", "urn:tth:", "urn:btih:", "urn:md5:" };
	quint8 lengths[5] = { 32 + 9, 32 + 9, 39 + 8, 32 + 9, 32 + 8 };

	for ( quint8 i = 0; i < 5; ++i )
	{
		sHash = "";

		pos1 = sContent.indexOf( prefixes[i] );
		if ( pos1 != -1 )
		{
			tmp  = sContent.mid( pos1 );
			pos2 = tmp.indexOf( ' ' );

			if ( pos2 == lengths[i] )
			{
				sHash = tmp.left( pos2 );
			}
			else if ( pos2 == -1 && tmp.length() == lengths[i] )
			{
				sHash = tmp;
			}
			else
			{
//				theApp.Message( MSG_ERROR, IDS_SECURITY_XML_HASH_RULE_IMPORT_ERROR );
				continue;
			}

			lHashes.append( *CHash::FromURN( sHash ) );
		}
	}

	if ( !lHashes.isEmpty() )
	{
		setContent( lHashes );
	}
	else
	{
//		theApp.Message( MSG_ERROR, IDS_SECURITY_XML_HASH_RULE_IMPORT_FAIL );
	}
}

bool CHashRule::hashEquals(CHashRule& oRule) const
{
	if ( oRule.m_Hashes.size() != m_Hashes.size() )
		return false;

	QMap< CHash::Algorithm, CHash >::const_iterator i, j;

	i = m_Hashes.begin();
	j = oRule.m_Hashes.begin();

	while ( i != m_Hashes.end() )
	{
		if ( *i != *j )
			return false;

		++i;
		++j;
	}

	return true;
}

bool CHashRule::match(const CFile& oFile) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentHash );
#endif //_DEBUG

	if ( oFile.isNull() )
		return false;

	return match( oFile.getHashes() );
}
bool CHashRule::match(const QList< CHash >& hashes) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentHash );
#endif //_DEBUG

	QMap< CHash::Algorithm, CHash >::const_iterator i;
	quint8 nCount = 0;

	foreach ( CHash oHash, hashes )
	{
		i = m_Hashes.find( oHash.getAlgorithm() );

		if ( i != m_Hashes.end() )
		{
			++nCount;

			if ( oHash != *i )
				return false;
		}
	}

	return nCount;
}

void CHashRule::toXML( QDomElement& oXMLroot ) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentHash );
#endif //_DEBUG

	QDomElement oElement = oXMLroot.ownerDocument().createElement( "rule" );

	oElement.setAttribute( "type", "hash" );
	oElement.setAttribute( "content", getContentString() );

	CSecureRule::toXML( this, oElement );

	oXMLroot.appendChild( oElement );
}

//////////////////////////////////////////////////////////////////////
// CRegExpRule

CRegExpRule::CRegExpRule(bool)
{
	m_nType = srContentRegExp;
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

void CRegExpRule::toXML( QDomElement& oXMLroot ) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentRegExp );
#endif //_DEBUG

	QDomElement oElement = oXMLroot.ownerDocument().createElement( "rule" );

	oElement.setAttribute( "type", "regexp" );
	oElement.setAttribute( "content", getContentString() );

	CSecureRule::toXML( this, oElement );

	oXMLroot.appendChild( oElement );
}

//////////////////////////////////////////////////////////////////////
// CUserAgentRule

CUserAgentRule::CUserAgentRule(bool)
{
	m_nType = srContentUserAgent;
	m_bRegExp  = false;
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

	oElement.setAttribute( "content", getContentString() );

	CSecureRule::toXML( this, oElement );

	oXMLroot.appendChild( oElement );
}

//////////////////////////////////////////////////////////////////////
// CContentRule

CContentRule::CContentRule(bool)
{
	m_nType = srContentAll;
}

void CContentRule::setContentString(const QString& strContent)
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

	m_sContent.clear();
	for ( CListIterator i = m_lContent.begin() ; i != m_lContent.end() ; i++ )
	{
		m_sContent = m_sContent + *i;
	}
}

void CContentRule::setAll(bool all)
{
	m_nType = all ? srContentAll : srContentAny;
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
bool CContentRule::match(const CFile& oFile) const
{
#ifdef _DEBUG
	Q_ASSERT( !oFile.isNull() && ( m_nType == srContentAny || m_nType == srContentAll ) );
#endif //_DEBUG

	if ( !oFile.isNull() )
	{
		if ( oFile.size() )
		{
			qint32 index = oFile.fileName().lastIndexOf( '.' );
			if ( index != -1 )
			{
				QString strExt = oFile.fileName().mid( index );
				QString strExtFileSize = "size:%1:%2";
				strExtFileSize.arg( strExt, QString::number( oFile.size() ) );
				if ( match( strExtFileSize ) )
					return true;
			}
		}

		if ( match( oFile.fileName() ) )
			return true;
	}
	return false;
}

void CContentRule::toXML( QDomElement& oXMLroot ) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentAll || m_nType == srContentAny );
#endif //_DEBUG

	QDomElement oElement = oXMLroot.ownerDocument().createElement( "rule" );

	oElement.setAttribute( "type", "content" );
	oElement.setAttribute( "content", getContentString() );

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


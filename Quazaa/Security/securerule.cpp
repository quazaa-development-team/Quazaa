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

#include "security.h"
#include "securerule.h"
#include "geoiplist.h"

#include "debug_new.h"

using namespace Security;

CSecureRule::CSecureRule(bool bCreate)
{
	// This invalidates rule as long as it does not contain any useful content.
	m_nType		= srContentUndefined;

	m_nAction	= srDeny;
	m_tExpire	= srIndefinite;
	m_nToday	= 0;
	m_nTotal	= 0;

	if ( bCreate )
		m_oUUID = QUuid::createUuid();
	else
		m_oUUID = QUuid();
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

bool CSecureRule::operator==(const CSecureRule& pRule) const
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
#ifdef _DEBUG
	Q_ASSERT( m_nType != srContentUndefined );
#endif //_DEBUG

	return m_sContent;
}

CSecureRule* CSecureRule::getCopy() const
{
	// This method should never be called.
#ifdef _DEBUG
	Q_ASSERT( false );
#endif //_DEBUG

	return new CSecureRule( *this );
}

void CSecureRule::registerPointer(CSecureRule** pRule)
{
	m_lPointers.push_back( pRule );
}

void CSecureRule::unRegisterPointer(CSecureRule** pRule)
{
	m_lPointers.remove( pRule );
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
	oStream << pRule->m_nTotal;

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
		pRule = new CCountryRule();
		fsFile >> sTmp;
		pRule->parseContent( sTmp );
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
// TODO: There is an empty or erroneous rule. Error handling (if necessary) should go here.
// TODO: Inform the user.
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
		const QString sMask    = attributes.value( "mask" ).toString();

		if ( sMask.isEmpty() )
		{
			pRule = new CIPRule();
			pRule->parseContent( sAddress );
		}
		else
		{
			pRule = new CIPRangeRule();
			sAddress = sAddress + '/' + sMask;
			pRule->parseContent( sAddress );
		}

		/*QHostAddress oIP;
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

					if ( sMask.isEmpty() || ( pos == -1 && ( i != 3 || sMask.length() > 3 ) ) || !pos || pos > 2 )
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
			else if ( oIP.protocol() == QAbstractSocket::IPv6Protocol )
			{
				if ( sMask )
				{
					CIPRangeRule* rule = new CIPRangeRule();

				}
				else
				{
					CIPRule* rule = new CIPRule();
					rule->setIP( oIP );

					pRule = rule;
				}

			}
			else
			{
				return NULL;
			}
		}
		else
		{
			// Error in XML entry.
			return NULL;
		}*/
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

//////////////////////////////////////////////////////////////////////
// CIPRule

CIPRule::CIPRule(bool)
{
	m_nType = srContentAddress;
}

bool CIPRule::parseContent(const QString& sContent)
{
	QHostAddress oAddress;
	if ( oAddress.setAddress( sContent ) )
	{
		m_oIP = oAddress;
		m_sContent = oAddress.toString();
		return true;
	}
	return false;
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

void CIPRule::toXML(QXmlStreamWriter& oXMLdocument) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentAddress );
#endif //_DEBUG

	oXMLdocument.writeStartElement( "rule" );

	oXMLdocument.writeAttribute( "type", "address" );
	oXMLdocument.writeAttribute( "address", getContentString() );

	CSecureRule::toXML( *this, oXMLdocument );

	oXMLdocument.writeEndElement();
}

//////////////////////////////////////////////////////////////////////
// CIPRangeRule

CIPRangeRule::CIPRangeRule(bool)
{
	m_nType = srContentAddressRange;
}

bool CIPRangeRule::parseContent(const QString& sContent)
{
	QPair<QHostAddress, int> oPair = QHostAddress::parseSubnet( sContent );

	if ( oPair != qMakePair( QHostAddress(), -1 ) )
	{
		m_oSubNet = oPair;
		m_sContent = m_oSubNet.first.toString() + "/" + QString::number( m_oSubNet.second );
		return true;
	}
	return false;
}

void CIPRangeRule::toXML(QXmlStreamWriter& oXMLdocument) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentAddressRange );
#endif //_DEBUG

	oXMLdocument.writeStartElement( "rule" );

	oXMLdocument.writeAttribute( "type", "address" );
	oXMLdocument.writeAttribute( "address", m_oSubNet.first.toString() );
	oXMLdocument.writeAttribute( "mask", QString( m_oSubNet.second ) );

	CSecureRule::toXML( *this, oXMLdocument );

	oXMLdocument.writeEndElement();
}

//////////////////////////////////////////////////////////////////////
// CCountryRule

CCountryRule::CCountryRule(bool)
{
	m_nType = srContentCountry;
}

bool CCountryRule::parseContent(const QString& sContent)
{
	if ( GeoIP.countryNameFromCode( sContent ) != "Unknown" )
	{
		m_sContent = sContent;
		return true;
	}
	return false;
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

void CCountryRule::toXML(QXmlStreamWriter& oXMLdocument) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentCountry );
#endif //_DEBUG

	oXMLdocument.writeStartElement( "rule" );

	oXMLdocument.writeAttribute( "type", "country" );
	oXMLdocument.writeAttribute( "content", getContentString() );

	CSecureRule::toXML( *this, oXMLdocument );

	oXMLdocument.writeEndElement();
}

//////////////////////////////////////////////////////////////////////
// CHashRule

CHashRule::CHashRule(bool)
{
	m_nType = srContentHash;
}

QList< CHash > CHashRule::getHashes() const
{
	QList< CHash > result;
	foreach ( CHash oHash, m_Hashes )
	{
		result.append( oHash );
	}

	return result;
}

void CHashRule::setHashes(const QList< CHash >& hashes)
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

bool CHashRule::parseContent(const QString& sContent)
{
	QString tmp, sHash;
	int pos1, pos2;
	QList< CHash > lHashes;

	QString prefixes[5] = { "urn:sha1:", "urn:ed2k:", "urn:tree:tiger:", "urn:btih:", "urn:md5:" };
	quint8 lengths[5] = { 32 + 9, 32 + 9, 39 + 15, 32 + 9, 32 + 8 };

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
		setHashes( lHashes );
		return true;
	}
	else
	{
//		theApp.Message( MSG_ERROR, IDS_SECURITY_XML_HASH_RULE_IMPORT_FAIL );
		return false;
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
		j = oRule.m_Hashes.find( (*i).getAlgorithm() );
		if ( *i != *j )
			return false;

		++i;
	}

	return true;
}

bool CHashRule::match(const CQueryHit* const pHit) const
{
	return match( pHit->m_lHashes );
}
bool CHashRule::match(const QList<CHash>& lHashes) const
{
	QMap< CHash::Algorithm, CHash >::const_iterator i;
	quint8 nCount = 0;

	foreach ( CHash oHash, lHashes )
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

void CHashRule::toXML(QXmlStreamWriter& oXMLdocument) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentHash );
#endif //_DEBUG

	oXMLdocument.writeStartElement( "rule" );

	oXMLdocument.writeAttribute( "type", "hash" );
	oXMLdocument.writeAttribute( "content", getContentString() );

	CSecureRule::toXML( *this, oXMLdocument );

	oXMLdocument.writeEndElement();
}

//////////////////////////////////////////////////////////////////////
// CRegExpRule

CRegExpRule::CRegExpRule(bool)
{
	m_nType = srContentRegExp;
	m_bSpecialElements = false;
}

bool CRegExpRule::operator==(const CSecureRule& pRule) const
{
	return CSecureRule::operator==( pRule ) &&
		   m_bSpecialElements == ((CRegExpRule*)&pRule)->m_bSpecialElements;
}

bool CRegExpRule::parseContent(const QString& sContent)
{
	m_sContent = sContent.trimmed();

	quint8 nCount = 0;
	for ( quint8 i = 1; i < 10; i++ )
	{
		if ( m_sContent.contains( "<" + QString::number( i ) + ">" ) )
		{
			nCount++;
		}
	}

	if ( nCount || m_sContent.contains( "<_>" ) || m_sContent.contains( "<>" ) )
	{
		// In this case the regular expression must be build ech time a filter request comes in,
		// so theres no point in doing it here.
		m_bSpecialElements = true;
		return true;
	}
	else
	{
		m_bSpecialElements = false;
		bool bValid;

#if QT_VERSION >= 0x050000
		m_regularExpressionContent = QRegularExpression( m_sContent );
		bValid = m_regularExpressionContent.isValid();
#else
		m_regExpContent = QRegExp( m_sContent );
		bValid = m_regExpContent.isValid();
#endif

		return bValid;
	}
}

bool CRegExpRule::match(const QList<QString>& lQuery, const QString& sContent) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentRegExp );
#endif //_DEBUG

	if ( m_sContent.isEmpty() )
		return false;

	if ( m_bSpecialElements )
	{
		// Build a regular expression filter from the search query words.
		// Returns an empty string if not applied or if the filter was invalid.
		//
		// Substitutes:
		// <_> - inserts all query keywords;
		// <0>..<9> - inserts query keyword number 0..9;
		// <> - inserts next query keyword.
		//
		// For example regular expression:
		//	.*(<2><1>)|(<_>).*
		// for "music mp3" query will be converted to:
		//	.*(mp3\s*music\s*)|(music\s*mp3\s*).*
		//
		// Note: \s* - matches any number of white-space symbols (including zero).

		QString sFilter, sBaseFilter = m_sContent;

		int pos = sBaseFilter.indexOf( '<' );
		if ( pos != -1 )
		{
			quint8 nArg = 0;

			// replace all relevant occurrences of <*something*
			while ( pos != -1 );
			{
				sFilter += sBaseFilter.left( pos );
				sBaseFilter.remove( 0, pos );
				bool bSuccess = replace( sBaseFilter, lQuery, nArg );

				pos = sBaseFilter.indexOf( '<', bSuccess ? 0 : 1 );
			}
			// add whats left of the base filter string to the newly generated filter
			sFilter += sBaseFilter;

#if QT_VERSION >= 0x050000
			QRegularExpression oRegExpFilter = QRegularExpression( sFilter );
			return oRegExpFilter.match( sContent ).hasMatch();
#else
			QRegExp oRegExpFilter = QRegExp( sFilter );
			return oRegExpFilter.exactMatch( sContent );
#endif
		}
		else
		{
			// This shouldn't happen, but it's covered anyway...
			Q_ASSERT( false );

#if QT_VERSION >= 0x050000
			QRegularExpression oRegExpFilter = QRegularExpression( m_sContent );
			return oRegExpFilter.match( sContent ).hasMatch();
#else
			QRegExp oRegExpFilter = QRegExp( m_sContent );
			return oRegExpFilter.exactMatch( sContent );
#endif
		}
	}
	else
	{
#if QT_VERSION >= 0x050000
		return m_regularExpressionContent.match( sContent ).hasMatch();
#else
		return m_regExpContent.exactMatch( sContent );
#endif
	}
}

void CRegExpRule::toXML(QXmlStreamWriter& oXMLdocument) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentRegExp );
#endif //_DEBUG

	oXMLdocument.writeStartElement( "rule" );

	oXMLdocument.writeAttribute( "type", "regexp" );
	oXMLdocument.writeAttribute( "content", getContentString() );

	CSecureRule::toXML( *this, oXMLdocument );

	oXMLdocument.writeEndElement();
}

bool CRegExpRule::replace(QString& sReplace, const QList<QString>& lQuery, quint8& nCurrent)
{
	if ( sReplace.at( 0 ) != '<' )
		return false;

	if ( sReplace.length() > 1 && sReplace.at( 1 ) == '>' )
	{
		sReplace.remove( 0, 2 );
		if ( nCurrent < lQuery.size() )
		{
			sReplace = lQuery.at( nCurrent ) + "\\s*" + sReplace;
		}
		nCurrent++;
		return true;
	}

	if ( sReplace.length() > 2 && sReplace.at( 2 ) == '>' )
	{
		if ( sReplace.at( 1 ) == '_' )
		{
			QString sMess;
			for ( quint8 i = 0; i < lQuery.size(); i++ )
			{
				sMess += lQuery.at( i );
				sMess += "\\s*";
			}
			sReplace.remove( 0, 3 );
			sReplace = sMess + sReplace;
			return true;
		}
		else
		{
			bool bSuccess;
			quint8 nArg = sReplace.mid( 1, 1 ).toShort( &bSuccess );
			if ( bSuccess )
			{
				sReplace.remove( 0, 3 );
				if ( nArg < lQuery.size() )
				{
					sReplace = lQuery.at( nArg ) + "\\s*" + sReplace;
				}
				return true;
			}
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
// CUserAgentRule

CUserAgentRule::CUserAgentRule(bool)
{
	m_nType = srContentUserAgent;
	m_bRegExp  = false;
}

bool CUserAgentRule::operator==(const CSecureRule& pRule) const
{
	return CSecureRule::operator==( pRule ) && m_bRegExp == ((CUserAgentRule*)&pRule)->m_bRegExp;
}

void CUserAgentRule::setRegExp(bool bRegExp)
{
	m_bRegExp = bRegExp;

	if ( m_bRegExp )
	{
#if QT_VERSION >= 0x050000
		m_regularExpressionContent = QRegularExpression( m_sContent );
#else
		m_regExpContent = QRegExp( m_sContent );
#endif
	}
}

bool CUserAgentRule::parseContent(const QString& sContent)
{
	m_sContent = sContent.trimmed();

	if ( m_bRegExp )
	{
#if QT_VERSION >= 0x050000
		m_regularExpressionContent = QRegularExpression( m_sContent );
#else
		m_regExpContent = QRegExp( m_sContent );
#endif
	}

	return true;
}

bool CUserAgentRule::match(const QString& sUserAgent) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentUserAgent );
#endif //_DEBUG

	if( m_bRegExp )
	{
#if QT_VERSION >= 0x050000
		return m_regularExpressionContent.match( sUserAgent ).hasMatch();
#else
		return m_regExpContent.exactMatch( sUserAgent );
#endif
	}
	else
	{
		return ( sUserAgent.indexOf( m_sContent ) != -1 );
	}

	return true;
}

void CUserAgentRule::toXML(QXmlStreamWriter& oXMLdocument) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentUserAgent );
#endif //_DEBUG

	oXMLdocument.writeStartElement( "rule" );

	oXMLdocument.writeAttribute( "type", "agent" );

	if( m_bRegExp )
	{
		oXMLdocument.writeAttribute( "match", "regexp" );
	}
	else
	{
		oXMLdocument.writeAttribute( "match", "list" );
	}

	oXMLdocument.writeAttribute( "content", getContentString() );

	CSecureRule::toXML( *this, oXMLdocument );

	oXMLdocument.writeEndElement();
}

//////////////////////////////////////////////////////////////////////
// CContentRule

CContentRule::CContentRule(bool)
{
	m_nType = srContentText;
	m_bAll = true;
}

bool CContentRule::operator==(const CSecureRule& pRule) const
{
	return CSecureRule::operator==( pRule ) && m_bAll == ((CContentRule*)&pRule)->m_bAll;
}

bool CContentRule::parseContent(const QString& sContent)
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentText );
#endif //_DEBUG

	QString sWork = sContent;
	sWork.replace( '\t', ' ' );

	m_lContent.clear();

	QString tmp;

	QList< QString > lWork;

	while ( !sWork.isEmpty() )
	{
		sWork = sWork.trimmed();
		int index = sWork.indexOf( ' ' );
		tmp = ( index != -1 ) ? sWork.left( index ) : sWork;
		if ( !tmp.isEmpty() )
			lWork.push_back( tmp );
		sWork = sWork.mid( ( index != -1 ) ? index : 0 );
	}

	if ( !lWork.isEmpty() )
	{
		m_lContent = lWork;

		m_sContent.clear();
		for ( CListIterator i = m_lContent.begin() ; i != m_lContent.end() ; i++ )
		{
			m_sContent += *i;
		}
		return true;
	}
	return false;
}

bool CContentRule::match(const QString& sFileName) const
{
	for ( CListIterator i = m_lContent.begin() ; i != m_lContent.end() ; i++ )
	{
		bool bFound = sFileName.indexOf( *i ) != -1;

		if ( bFound && !m_bAll )
		{
			return true;
		}
		else if ( !bFound && m_bAll )
		{
			return false;
		}
	}

	if ( m_bAll )
		return true;

	return false;
}

bool CContentRule::match(const CQueryHit* const pHit) const
{
	if ( !pHit )
		return false;

	QString sFileName = pHit->m_sDescriptiveName;
	qint32 index = sFileName.lastIndexOf( '.' );
	if ( index != -1 )
	{
		QString sExt = sFileName.mid( index );
		QString sExtFileSize = "size:%1:%2";
		sExtFileSize.arg( sExt, QString::number( pHit->m_nObjectSize ) );
		if ( match( sExtFileSize ) )
			return true;
	}

	return match( sFileName );
}

void CContentRule::toXML(QXmlStreamWriter& oXMLdocument) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentText );
#endif //_DEBUG

	oXMLdocument.writeStartElement( "rule" );

	oXMLdocument.writeAttribute( "type", "content" );

	if( m_bAll )
	{
		oXMLdocument.writeAttribute( "match", "all" );
	}
	else
	{
		oXMLdocument.writeAttribute( "match", "any" );
	}

	oXMLdocument.writeAttribute( "content", getContentString() );

	CSecureRule::toXML( *this, oXMLdocument );

	oXMLdocument.writeEndElement();
}

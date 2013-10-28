#include "hashrule.h"

CHashRule::CHashRule()
{
	m_nType = RuleType::Hash;
}

CSecureRule* CHashRule::getCopy() const
{
	return new CHashRule( *this );
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
	Q_ASSERT( m_nType == RuleType::Hash );

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
	QList<CHash> lHashes;

	QStringList prefixes;
	prefixes << "urn:sha1:" << "urn:ed2k:" << "urn:ed2khash:" << "urn:tree:tiger:" << "urn:btih:" << "urn:bitprint:" << "urn:md5:";
	QList<int> lengths;
	lengths << 32 << 32 << 32 << 39 << 32 << 72 << 32;

	for ( int i = 0; i < prefixes.size(); ++i )
	{
		QString tmp, sHash;
		int pos1, pos2;

		pos1 = sContent.indexOf( prefixes.at(i) );
		if ( pos1 != -1 )
		{
			tmp  = sContent.mid( pos1 );
			int length = lengths.at(i) + prefixes.at(i).length();
			pos2 = tmp.indexOf( "&" );

			if ( pos2 == length )
			{
				systemLog.postLog(LogSeverity::Information, Components::Security, tr("Hash found for hash rule: %1").arg(tmp.left( pos2 )));
				sHash = tmp.left( pos2 );
			}
			else if ( pos2 == -1 && tmp.length() == length )
			{
				systemLog.postLog(LogSeverity::Information, Components::Security, tr("Hash found for hash rule at end of string: %1").arg(tmp.left( pos2 )));
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
	Q_ASSERT( m_nType == RuleType::Hash );

	oXMLdocument.writeStartElement( "rule" );

	oXMLdocument.writeAttribute( "type", "hash" );
	oXMLdocument.writeAttribute( "content", getContentString() );

	CSecureRule::toXML( *this, oXMLdocument );

	oXMLdocument.writeEndElement();
}

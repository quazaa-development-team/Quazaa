#include "useragentrule.h"

CUserAgentRule::CUserAgentRule()
{
	m_nType = RuleType::UserAgent;
	m_bRegExp  = false;
}

CSecureRule* CUserAgentRule::getCopy() const
{
	return new CUserAgentRule( *this );
}

bool CUserAgentRule::getRegExp() const
{
	return m_bRegExp;
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
	Q_ASSERT( m_nType == RuleType::UserAgent );

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

bool CUserAgentRule::partialMatch(const QString &sUserAgent) const
{
	//TODO: Figure out why this is triggered when removing an agent rule
	Q_ASSERT( m_nType == RuleType::UserAgent );

	return sUserAgent.contains( m_sContent, Qt::CaseInsensitive );
}

void CUserAgentRule::toXML(QXmlStreamWriter& oXMLdocument) const
{
	Q_ASSERT( m_nType == RuleType::UserAgent );

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

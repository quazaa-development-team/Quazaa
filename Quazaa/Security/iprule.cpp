#include "iprule.h"

CIPRule::CIPRule()
{
	m_nType = srContentAddress;
}

QHostAddress CIPRule::IP() const
{
	return m_oIP;
}

void CIPRule::setIP( const QHostAddress& oIP )
{
	m_oIP = oIP;
	m_sContent = oIP.toString();
}

CSecureRule* CIPRule::getCopy() const
{
	return new CIPRule( *this );
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

bool CIPRule::match(const CEndPoint& oAddress) const
{
	Q_ASSERT( !oAddress.isNull() && m_nType == srContentAddress );

	if ( !oAddress.isNull() && oAddress == m_oIP )
	{
		return true;
	}
	return false;
}

void CIPRule::toXML(QXmlStreamWriter& oXMLdocument) const
{
	Q_ASSERT( m_nType == srContentAddress );

	oXMLdocument.writeStartElement( "rule" );

	oXMLdocument.writeAttribute( "type", "address" );
	oXMLdocument.writeAttribute( "address", getContentString() );

	CSecureRule::toXML( *this, oXMLdocument );

	oXMLdocument.writeEndElement();
}

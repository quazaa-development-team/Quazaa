#include "countryrule.h"

#if SECURITY_ENABLE_GEOIP
#include "geoiplist.h"

CCountryRule::CCountryRule()
{
	m_nType = srContentCountry;
}

CSecureRule* CCountryRule::getCopy() const
{
	return new CCountryRule( *this );
}

bool CCountryRule::parseContent(const QString& sContent)
{
	if ( geoIP.countryNameFromCode( sContent ) != "Unknown" )
	{
		m_sContent = sContent;
		return true;
	}
	return false;
}

bool CCountryRule::match(const CEndPoint& oAddress) const
{
	Q_ASSERT( !oAddress.isNull() && m_nType == srContentCountry );

	if ( m_sContent == oAddress.country() )
		return true;

	return false;
}

void CCountryRule::toXML(QXmlStreamWriter& oXMLdocument) const
{
	Q_ASSERT( m_nType == srContentCountry );

	oXMLdocument.writeStartElement( "rule" );

	oXMLdocument.writeAttribute( "type", "country" );
	oXMLdocument.writeAttribute( "content", getContentString() );

	CSecureRule::toXML( *this, oXMLdocument );

	oXMLdocument.writeEndElement();
}
#endif // SECURITY_ENABLE_GEOIP

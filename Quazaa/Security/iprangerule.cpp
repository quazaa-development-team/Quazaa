#include "iprangerule.h"
#include <QDebug>

CIPRangeRule::CIPRangeRule()
{
	m_nType = srContentAddressRange;
}

QHostAddress CIPRangeRule::startIP() const
{
	return m_oStartIP;
}

QHostAddress CIPRangeRule::endIP() const
{
	return m_oEndIP;
}

CSecureRule* CIPRangeRule::getCopy() const
{
	return new CIPRangeRule( *this );
}

bool CIPRangeRule::parseContent(const QString& sContent)
{
	QStringList addresses = sContent.split("-");

	QHostAddress oStartAddress;
	QHostAddress oEndAddress;
	if ( oStartAddress.setAddress( addresses.at(0) ) && oEndAddress.setAddress( addresses.at(1) ) )
	{
		m_oStartIP = oStartAddress;
		m_oEndIP = oEndAddress;
		m_sContent = sContent;
		return true;
	}
	return false;
}

void CIPRangeRule::toXML(QXmlStreamWriter& oXMLdocument) const
{
	Q_ASSERT( m_nType == srContentAddressRange );

	oXMLdocument.writeStartElement( "rule" );

	oXMLdocument.writeAttribute( "type", "addressrange" );
	oXMLdocument.writeAttribute( "startaddress", m_oStartIP.toString() );
	oXMLdocument.writeAttribute( "endaddress", m_oEndIP.toString() );

	CSecureRule::toXML( *this, oXMLdocument );

	oXMLdocument.writeEndElement();
}

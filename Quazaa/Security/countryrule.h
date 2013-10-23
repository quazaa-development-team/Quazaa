#ifndef COUNTRYRULE_H
#define COUNTRYRULE_H

#include "securerule.h"

// Enable/disable GeoIP support of the library.
#define SECURITY_ENABLE_GEOIP 1

#if SECURITY_ENABLE_GEOIP
class CCountryRule : public CSecureRule
{
public:
	CCountryRule();

	bool				parseContent(const QString& sContent);

	CSecureRule*	getCopy() const;

	bool				match(const CEndPoint& oAddress) const;
	void				toXML(QXmlStreamWriter& oXMLdocument) const;
};
#endif // SECURITY_ENABLE_GEOIP

#endif // COUNTRYRULE_H

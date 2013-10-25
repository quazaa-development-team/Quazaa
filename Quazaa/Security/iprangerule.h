#ifndef IPRANGERULE_H
#define IPRANGERULE_H

#include "securerule.h"

class CIPRangeRule : public CSecureRule
{
private:
	CEndPoint m_oStartIP;
	CEndPoint m_oEndIP;

public:
	CIPRangeRule();

	CEndPoint startIP() const;
	CEndPoint endIP() const;

	bool parseContent(const QString& sContent);

	CSecureRule* getCopy() const;

	bool contains(const CEndPoint& oAddress) const;
	void toXML(QXmlStreamWriter& oXMLdocument) const;
};

#endif // IPRANGERULE_H

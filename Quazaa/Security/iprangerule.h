#ifndef IPRANGERULE_H
#define IPRANGERULE_H

#include "securerule.h"

class CIPRangeRule : public CSecureRule
{
private:
	QHostAddress m_oStartIP;
	QHostAddress m_oEndIP;

public:
	CIPRangeRule();

	void setStartIP(QHostAddress ip);
	QHostAddress	startIP() const;
	void setEndIP(QHostAddress ip);
	QHostAddress	endIP() const;

	bool				parseContent(const QString& sContent);

	CSecureRule*	getCopy() const;

	bool			match(const CEndPoint& oAddress) const;
	void				toXML(QXmlStreamWriter& oXMLdocument) const;
};

#endif // IPRANGERULE_H

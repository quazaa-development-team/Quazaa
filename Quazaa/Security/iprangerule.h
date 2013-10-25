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

	QHostAddress	startIP() const;
	QHostAddress	endIP() const;

	bool				parseContent(const QString& sContent);

	CSecureRule*	getCopy() const;

	void				toXML(QXmlStreamWriter& oXMLdocument) const;
};

#endif // IPRANGERULE_H

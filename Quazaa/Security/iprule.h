#ifndef IPRULE_H
#define IPRULE_H

#include "securerule.h"

class CIPRule : public CSecureRule
{
private:
	QHostAddress m_oIP;

public:
	CIPRule();

	QHostAddress IP() const;
	void			setIP( const QHostAddress& oIP );

	bool				parseContent(const QString& sContent);

	CSecureRule*	getCopy() const;

	bool				match(const CEndPoint& oAddress) const;
	void				toXML(QXmlStreamWriter& oXMLdocument) const;
};

#endif // IPRULE_H

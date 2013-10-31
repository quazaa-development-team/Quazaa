#ifndef IPRULE_H
#define IPRULE_H

#include "securerule.h"

class CIPRule : public CSecureRule
{
private:
	CEndPoint m_oIP;

public:
	CIPRule();

	CEndPoint IP() const;
	void			setIP(const CEndPoint &oIP );

	bool				parseContent(const QString& sContent);

	CSecureRule*	getCopy() const;

	bool				match(const CEndPoint& oAddress) const;
	void				toXML(QXmlStreamWriter& oXMLdocument) const;

	inline bool operator<(const CIPRule& rhs) const;
};

bool CIPRule::operator<(const CIPRule &rhs) const
{
	if( m_oIP.protocol() == QAbstractSocket::IPv4Protocol )
	{
		return m_oIP.toIPv4Address() < rhs.IP().toIPv4Address();
	}
	else
	{
		Q_IPV6ADDR thisAddr = m_oIP.toIPv6Address();
		Q_IPV6ADDR thatAddr = rhs.IP().toIPv6Address();

		int n = memcmp(&thisAddr, &thatAddr, sizeof(Q_IPV6ADDR));
		return n < 0;
	}
}

#endif // IPRULE_H

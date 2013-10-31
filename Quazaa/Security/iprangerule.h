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

	inline bool operator<(const CIPRangeRule& rhs) const;
};

bool CIPRangeRule::operator<(const CIPRangeRule &rhs) const
{
	if( m_oStartIP.protocol() == QAbstractSocket::IPv4Protocol )
	{
		return m_oStartIP.toIPv4Address() < rhs.startIP().toIPv4Address();
	}
	else
	{
		Q_IPV6ADDR thisAddr = m_oStartIP.toIPv6Address();
		Q_IPV6ADDR thatAddr = rhs.startIP().toIPv6Address();

		int n = memcmp(&thisAddr, &thatAddr, sizeof(Q_IPV6ADDR));
		return n < 0;
	}
}

#endif // IPRANGERULE_H

#ifndef ENDPOINT_H
#define RNDPOINT_H

#include <QHostAddress>

class CEndPoint : public QHostAddress
{
protected:
	quint16	m_nPort;

public:
	CEndPoint();
	explicit CEndPoint(quint32 ip4Addr, quint16 nPort = 0);
	explicit CEndPoint(quint8 *ip6Addr, quint16 nPort = 0);
	explicit CEndPoint(const Q_IPV6ADDR &ip6Addr, quint16 nPort = 0);
	explicit CEndPoint(const sockaddr *sockaddr, quint16 nPort = 0);
	explicit CEndPoint(const QString &address, quint16 nPort);
	explicit CEndPoint(const QString &address);
	CEndPoint(const CEndPoint &copy);
	CEndPoint(SpecialAddress address, quint16 nPort = 0);

public:
	void clear();
	QString toStringWithPort() const;
	quint16 port() const;
	void setPort(const quint16 nPort);
};

#endif // ENDPOINT_H

#ifndef CHATCORE_H
#define CHATCORE_H

#include "types.h"
#include <QObject>
#include "thread.h"

class CChatSession;
class CRateController;
class CNetworkConnection;

class CChatCore : public QObject
{
	Q_OBJECT
public:
	QMutex	m_pSection;
protected:
	QList<CChatSession*> m_lSessions;
	CRateController* m_pController;
public:
	CChatCore(QObject *parent = 0);
	virtual ~CChatCore();

	void OnAccept(CNetworkConnection* pConn, DiscoveryProtocol nProto);

protected:
	void Add(CChatSession* pSession);
	void Remove(CChatSession* pSession);

	void Start();
	void Stop();

protected slots:
	void OnTimer();

public:

signals:
	void openChatWindow(CChatSession*);

public slots:


	friend class CChatSession;
};

extern CChatCore ChatCore;
extern CThread ChatThread;
#endif // CHATCORE_H

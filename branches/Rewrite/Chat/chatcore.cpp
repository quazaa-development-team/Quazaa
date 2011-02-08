#include "chatcore.h"
#include "ratecontroller.h"
#include "chatsession.h"

CChatCore ChatCore;
CThread ChatThread;

CChatCore::CChatCore(QObject *parent) :
	QObject(parent), m_bActive(false)
{
}
CChatCore::~CChatCore()
{
	if( m_bActive )
		Stop();
}


void CChatCore::Add(CChatSession *pSession)
{
	QMutexLocker l(&m_pSection);

	if( !m_lSessions.contains(pSession) )
		m_lSessions.append(pSession);

	Start();

	m_pController->AddSocket(pSession);
}
void CChatCore::Remove(CChatSession *pSession)
{
	QMutexLocker l(&m_pSection);

	if( int nSession = m_lSessions.indexOf(pSession) != -1 )
	{
		m_lSessions.removeAt(nSession);
		m_pController->RemoveSocket(pSession);
	}
}

void CChatCore::Start()
{
	if( m_bActive )
		return;

	ChatThread.start("ChatCore", &m_pSection);
	m_bActive = true;

	m_pController = new CRateController(&m_pSection);
	m_pController->SetDownloadLimit(8192);
	m_pController->SetUploadLimit(8192);
	m_pController->moveToThread(&ChatThread);
}
void CChatCore::Stop()
{
	ChatThread.exit(0);
	m_bActive = false;

	qDeleteAll(m_lSessions);
	m_lSessions.clear();
	delete m_pController;
}

void CChatCore::OnTimer()
{

}

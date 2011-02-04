#include "dialogprivatemessages.h"
#include "ui_dialogprivatemessages.h"

#include "widgetprivatemessage.h"
#include "Chat/chatcore.h"

#include <QTextDocument>

DialogPrivateMessages::DialogPrivateMessages(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogPrivateMessages)
{
	ui->setupUi(this);
	widgetChatInput = new WidgetChatInput(this);
	ui->horizontalLayoutChatInput->addWidget(widgetChatInput);
	connect(widgetChatInput, SIGNAL(messageSent(QTextDocument*)), this, SLOT(onMessageSent(QTextDocument*)));
	m_pCurrentWidget = 0;
	qRegisterMetaType<QUuid>("QUuid");
}

DialogPrivateMessages::~DialogPrivateMessages()
{
	delete widgetChatInput;
	delete ui;
}

WidgetPrivateMessage* DialogPrivateMessages::FindByGUID(QUuid oGUID)
{
	foreach( WidgetPrivateMessage* pWg, m_lChatWindows )
	{
		if( oGUID == pWg->m_oGUID )
			return pWg;
	}

	return 0;
}

void DialogPrivateMessages::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void DialogPrivateMessages::OpenChat(CChatSession* pSess)
{
	ChatCore.m_pSection.lock();
	WidgetPrivateMessage* pWg = FindByGUID(pSess->m_oGUID);
	if( pWg == 0 )
	{
		pWg = new WidgetPrivateMessage(this);
		connect(pWg, SIGNAL(destroyed(QObject*)), this, SLOT(onWindowDestroyed(QObject*)));
		m_lChatWindows.append(pWg);
		int idx = ui->tabWidgetPrivateMessages->addTab(pWg, pSess->m_oAddress.toStringWithPort());
		m_pCurrentWidget = pWg;
		ui->tabWidgetPrivateMessages->setCurrentIndex(idx);
	}

	pSess->SetupWidget(pWg);
	pWg->show();
	ChatCore.m_pSection.unlock();
}

void DialogPrivateMessages::onMessageSent(QTextDocument *pDoc)
{
	QString sMessage = pDoc->toPlainText();

	if( sMessage.isEmpty() )
		return;

	if( sMessage.left(2) != "//" )
	{
		if( sMessage.left(1) == "/" ) // is it a command?
		{
			QString sCmd, sParam;
			int nSpace = sMessage.indexOf(" ");
			if( nSpace == -1 )
				nSpace = sMessage.length();

			sCmd = sMessage.left(nSpace).toLower();
			sParam = sMessage.mid(nSpace + 1);

			if( sCmd == "/me" )
			{
				if( !sParam.isEmpty() )
					m_pCurrentWidget->SendMessage(sParam, true);
			}
			else
			{
				m_pCurrentWidget->OnSystemMessage("Unknown command");
			}

			return;
		}
	}
	m_pCurrentWidget->SendMessage(pDoc, false);
}

void DialogPrivateMessages::onWindowDestroyed(QObject *pWg)
{
	int index = m_lChatWindows.indexOf(qobject_cast<WidgetPrivateMessage*>(pWg));
	m_lChatWindows.removeAt(index);

	if( m_lChatWindows.isEmpty() )
	{
		delete this;
	}
}

void DialogPrivateMessages::on_tabWidgetPrivateMessages_currentChanged(int index)
{
	m_pCurrentWidget = m_lChatWindows[index];
}

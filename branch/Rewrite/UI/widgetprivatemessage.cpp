#include "widgetprivatemessage.h"
#include "ui_widgetprivatemessage.h"
#include "quazaasettings.h"
#include "Chat/chatconverter.h"

WidgetPrivateMessage::WidgetPrivateMessage(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WidgetPrivateMessage)
{
	ui->setupUi(this);
	m_pSession = 0;
}

WidgetPrivateMessage::~WidgetPrivateMessage()
{
	delete ui;
}

void WidgetPrivateMessage::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void WidgetPrivateMessage::OnIncomingMessage(QString sMessage, bool bAction)
{
	qDebug() << "incoming message: " << sMessage;

	if( bAction )
	{
		ui->textEdit->insertHtml("* " + m_sNick + " " + sMessage);
	}
	else
	{
		ui->textEdit->insertHtml("&lt;" + m_sNick + "&gt;: " + sMessage);
	}
	ui->textEdit->insertHtml("<br />");
}
void WidgetPrivateMessage::OnSystemMessage(QString sMessage)
{
	qDebug() << "system message: " << sMessage;

	ui->textEdit->insertHtml("<font color=\"#FF0000\"><b>[SYSTEM]</b> " + sMessage + "</font>");
	ui->textEdit->insertHtml("<br />");
}
void WidgetPrivateMessage::OnGUIDChanged(QUuid oGUID)
{
	m_oGUID = oGUID;
}
void WidgetPrivateMessage::OnNickChanged(QString sNick)
{
	OnSystemMessage(m_sNick + " is now known as " + sNick);
	m_sNick = sNick;
	ui->label->setText(m_sNick);
}

void WidgetPrivateMessage::SendMessage(QString sMessage, bool bAction)
{
	if( bAction )
	{
		ui->textEdit->insertHtml("* " + quazaaSettings.Profile.GnutellaScreenName + " " + sMessage);
	}
	else
	{
		ui->textEdit->insertHtml("&lt;" + quazaaSettings.Profile.GnutellaScreenName + "&gt;: " + sMessage);
	}

	ui->textEdit->insertHtml("<br />");

	emit SendMessageS(sMessage, bAction);
}

void WidgetPrivateMessage::SendMessage(QTextDocument *pMessage, bool bAction)
{
	CChatConverter oConv(pMessage);

	if( bAction )
	{
		ui->textEdit->insertHtml("* " + quazaaSettings.Profile.GnutellaScreenName + " " + oConv.toHtml());
	}
	else
	{
		ui->textEdit->insertHtml("&lt;" + quazaaSettings.Profile.GnutellaScreenName + "&gt;: " + oConv.toHtml());
	}

	ui->textEdit->insertHtml("<br />");

	emit SendMessageS(pMessage->clone(), bAction);
}

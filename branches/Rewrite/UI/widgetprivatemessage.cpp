#include "widgetprivatemessage.h"
#include "ui_widgetprivatemessage.h"
#include "quazaasettings.h"
#include "Chat/chatconverter.h"

#include <QDesktopServices>
#include <QScrollBar>

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

void WidgetPrivateMessage::on_textEdit_anchorClicked(QUrl link)
{
	QDesktopServices::openUrl(link);
}

void WidgetPrivateMessage::OnIncomingMessage(QString sMessage, bool bAction)
{
	qDebug() << "incoming message: " << sMessage;
	if( bAction )
	{
		ui->textEdit->append("* " + m_sNick + " " + sMessage);
	}
	else
	{
		ui->textEdit->append("&lt;" + m_sNick + "&gt;: " + sMessage);
	}
}
void WidgetPrivateMessage::OnSystemMessage(QString sMessage)
{
	qDebug() << "system message: " << sMessage;

	ui->textEdit->append("<font color=\"#FF0000\"><b>[SYSTEM]</b> " + sMessage + "</font>");
}
void WidgetPrivateMessage::OnGUIDChanged(QUuid oGUID)
{
	m_oGUID = oGUID;
}
void WidgetPrivateMessage::OnNickChanged(QString sNick)
{
	OnSystemMessage(m_sNick + " is now known as " + sNick);
	m_sNick = sNick;
	ui->labelName->setText(m_sNick);
}

void WidgetPrivateMessage::SendMessage(QString sMessage, bool bAction)
{
	if( bAction )
	{
		ui->textEdit->append("* " + quazaaSettings.Profile.GnutellaScreenName + " " + sMessage);
	}
	else
	{
		ui->textEdit->append("&lt;" + quazaaSettings.Profile.GnutellaScreenName + "&gt;: " + sMessage);
	}

	emit SendMessageS(sMessage, bAction);
}

void WidgetPrivateMessage::SendMessage(QTextDocument *pMessage, bool bAction)
{
	CChatConverter oConv(pMessage);

	if( bAction )
	{
		ui->textEdit->append("* " + quazaaSettings.Profile.GnutellaScreenName + " " + oConv.toHtml());
	}
	else
	{
		ui->textEdit->append("&lt;" + quazaaSettings.Profile.GnutellaScreenName + "&gt;: " + oConv.toHtml());
	}

	emit SendMessageS(pMessage->clone(), bAction);
}

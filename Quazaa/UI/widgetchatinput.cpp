/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2012.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "widgetchatinput.h"
#include "ui_widgetchatinput.h"
#include "dialogconnectto.h"
#include "dialogirccolordialog.h"
#include "skinsettings.h"

#include "chatsessiong2.h"

#include <QColorDialog>
#include <QPalette>

#ifdef _DEBUG
#include "debug_new.h"
#endif

WidgetChatInput::WidgetChatInput(QWidget *parent, bool isIrc) :
	QMainWindow(parent),
	ui(new Ui::WidgetChatInput)
{
	ui->setupUi(this);
	bIsIrc = isIrc;
	textEditInput = new WidgetReturnEmitTextEdit(this);

	QTextCharFormat format;
	format.setFontStyleHint(QFont::TypeWriter);
	textEditInput->setCurrentCharFormat(format);

	connect(textEditInput, SIGNAL(cursorPositionChanged()), this, SLOT(updateToolbar()));
	ui->horizontalLayoutInput->addWidget(textEditInput);
	checkBoxSendOnEnter = new QCheckBox(tr("Send On Enter"), this);
	checkBoxSendOnEnter->setChecked(true);
	connect(checkBoxSendOnEnter, SIGNAL(toggled(bool)), textEditInput, SLOT(setEmitsReturn(bool)));
	connect(textEditInput, SIGNAL(returnPressed()), ui->toolButtonSend, SLOT(click()));
	connect(textEditInput, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SLOT(onTextFormatChange(QTextCharFormat)));
	toolButtonSmilies = new QToolButton();
	toolButtonSmilies->setPopupMode(QToolButton::InstantPopup);
	toolButtonSmilies->setToolTip(tr("Smilies"));
	toolButtonSmilies->setIcon(QIcon(":/Resource/Smileys/0.png"));
	widgetSmileyList = new WidgetSmileyList(this);
	toolButtonSmilies->setMenu(widgetSmileyList);

	toolButtonPickColor = new QToolButton(this);
	toolButtonPickColor->setIconSize(QSize(24,24));
	if(bIsIrc)
		toolButtonPickColor->setIcon(QIcon(":/Resource/Generic/Skin.png"));
	else
		toolButtonPickColor->setStyleSheet(QString("QToolButton { background-color: %1; border-style: outset; border-width: 2px;	border-radius: 6px; border-color: lightgrey; }").arg(textEditInput->textColor().name()));
	toolButtonPickColor->setToolTip(tr("Font Color"));
	connect(toolButtonPickColor, SIGNAL(clicked()), this, SLOT(pickColor()));

	toolButtonPrivateMessage = new QToolButton(this);
	toolButtonPrivateMessage->setText(tr("New Private Message"));
	toolButtonPrivateMessage->setToolTip(tr("New Private Message"));
	toolButtonPrivateMessage->setIcon(QIcon(":/Resource/Chat/Chat.png"));
	ui->toolBarTextTools->insertWidget(ui->actionBold, toolButtonPickColor);
	ui->toolBarTextTools->addSeparator();
	ui->toolBarTextTools->addWidget(toolButtonSmilies);
	ui->toolBarTextTools->addWidget(checkBoxSendOnEnter);
	ui->actionBold->setChecked(textEditInput->fontWeight() == QFont::Bold);
	ui->actionItalic->setChecked(textEditInput->fontItalic());
	ui->actionUnderline->setChecked(textEditInput->fontUnderline());
	ui->toolBarTextTools->addWidget(toolButtonPrivateMessage);
	toolButtonPrivateMessage->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	connect(ui->actionItalic, SIGNAL(toggled(bool)), textEditInput, SLOT(setFontItalic(bool)));
	connect(ui->actionUnderline, SIGNAL(toggled(bool)), textEditInput, SLOT(setFontUnderline(bool)));
	connect(toolButtonPrivateMessage, SIGNAL(clicked()), this, SLOT(addPrivateMessage()));
	setSkin();
}

WidgetChatInput::~WidgetChatInput()
{
	delete widgetSmileyList;
	delete ui;
}

void WidgetChatInput::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void WidgetChatInput::on_toolButtonSend_clicked()
{
	if (!textEditInput->document()->isEmpty())
	{
		if (textEditInput->document()->lineCount() > 1)
		{
			QStringList lineList = textEditInput->document()->toHtml().split("\n");
			for(int i = 4; i < lineList.size(); i++)
			{
				QTextDocument *line = new QTextDocument();
				line->setHtml(lineList.at(i));
				if(line->toPlainText().startsWith("/"))
					emit messageSent(line->toPlainText());
				else
					emit messageSent(line);
			}
		} else {
			if(textEditInput->document()->toPlainText().startsWith("/"))
				emit messageSent(textEditInput->document()->toPlainText());
			else
				emit messageSent(textEditInput->document());
		}
		textEditInput->addHistory(textEditInput->document());
		textEditInput->resetHistoryIndex();
		QTextCharFormat oldFormat = textEditInput->currentCharFormat();
		textEditInput->document()->clear();
		textEditInput->setCurrentCharFormat(oldFormat);
	}
}

void WidgetChatInput::setText(QString text)
{
	textEditInput->setHtml(text);
}

void WidgetChatInput::onTextFormatChange(QTextCharFormat newFormat)
{
	if( newFormat.fontWeight() == QFont::Normal )
		ui->actionBold->setChecked(false);
	else if( newFormat.fontWeight() == QFont::Bold )
		ui->actionBold->setChecked(true);

	ui->actionItalic->setChecked(newFormat.fontItalic());
	ui->actionUnderline->setChecked(newFormat.fontUnderline());
}

void WidgetChatInput::on_actionBold_toggled(bool checked)
{
	textEditInput->setFontWeight((checked ? QFont::Bold : QFont::Normal));
}

void WidgetChatInput::on_actionItalic_toggled(bool checked)
{
	textEditInput->setFontItalic(checked);
}

void WidgetChatInput::on_actionUnderline_toggled(bool checked)
{
	textEditInput->setFontUnderline(checked);
}

void WidgetChatInput::pickColor()
{
	QColor fontColor;
	if (bIsIrc)
	{
		DialogIrcColorDialog *dlgIrcColor = new DialogIrcColorDialog(textEditInput->textColor(), this);
		bool accepted = dlgIrcColor->exec();
		if (accepted)
		{
			if (!dlgIrcColor->isDefaultColor())
			{
				fontColor = dlgIrcColor->color();
				textEditInput->setTextColor(fontColor);
				toolButtonPickColor->setIcon(QIcon());
				toolButtonPickColor->setStyleSheet(QString("QToolButton { background-color: %1; border-style: outset; border-width: 2px;	border-radius: 6px; border-color: lightgrey; }").arg(fontColor.name()));
			}
			else
			{
				fontColor = qApp->palette().text().color();
				textEditInput->setTextColor(fontColor);
				toolButtonPickColor->setIcon(QIcon(":/Resource/Generic/Skin.png"));
				toolButtonPickColor->setStyleSheet("");
			}
		}
	}
	else
	{
		fontColor = QColorDialog::getColor(textEditInput->textColor(), this, tr("Select Font Color"));

		if (fontColor.isValid())
		{
			textEditInput->setTextColor(fontColor);
			toolButtonPickColor->setStyleSheet(QString("QToolButton { background-color: %1; border-style: outset; border-width: 2px;	border-radius: 6px; border-color: lightgrey; }").arg(fontColor.name()));
		}
	}
}

void WidgetChatInput::addPrivateMessage()
{
	DialogConnectTo* dlgConnectTo = new DialogConnectTo(this);
	bool accepted = dlgConnectTo->exec();

	if (accepted)
	{
		CEndPoint ip(dlgConnectTo->getAddressAndPort());

		switch (dlgConnectTo->getConnectNetwork())
		{
		case DialogConnectTo::G2:
		{
			CChatSessionG2* pS = new CChatSessionG2(ip);
			pS->Connect();
			break;
		}
		case DialogConnectTo::eDonkey:
			break;
		case DialogConnectTo::Ares:
			break;
		default:
			break;
		}
	}
}

void WidgetChatInput::updateToolbar()
{
	if(bIsIrc && (textEditInput->textColor() != qApp->palette().text().color()) )
	{
		toolButtonPickColor->setIcon(QIcon());
		toolButtonPickColor->setStyleSheet(QString("QToolButton { background-color: %1; border-style: outset; border-width: 2px;	border-radius: 6px; border-color: lightgrey; }").arg(textEditInput->textColor().name()));
	} else if (bIsIrc) {
		toolButtonPickColor->setIcon(QIcon(":/Resource/Generic/Skin.png"));
		toolButtonPickColor->setStyleSheet("");
	} else {
		toolButtonPickColor->setStyleSheet(QString("QToolButton { background-color: %1; border-style: outset; border-width: 2px;	border-radius: 6px; border-color: lightgrey; }").arg(textEditInput->textColor().name()));
	}
	ui->actionBold->setChecked(textEditInput->fontWeight() == QFont::Bold);
	ui->actionItalic->setChecked(textEditInput->fontItalic());
	ui->actionUnderline->setChecked(textEditInput->fontUnderline());
}

WidgetReturnEmitTextEdit *WidgetChatInput::textEdit()
{
	return textEditInput;
}

QLabel *WidgetChatInput::helpLabel()
{
	return ui->helpLabel;
}

void WidgetChatInput::setSkin()
{

}

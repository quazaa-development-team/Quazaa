#include "widgetchatinput.h"
#include "ui_widgetchatinput.h"

#include <QColorDialog>

WidgetChatInput::WidgetChatInput(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetChatInput)
{
	ui->setupUi(this);
	textEditInput = new WidgetReturnEmitTextEdit(this);
	ui->horizontalLayoutInput->addWidget(textEditInput);
	checkBoxSendOnEnter = new QCheckBox(tr("Send On Enter"), this);
	checkBoxSendOnEnter->setChecked(true);
	connect(checkBoxSendOnEnter, SIGNAL(toggled(bool)), textEditInput, SLOT(setEmitsReturn(bool)));
	connect(textEditInput, SIGNAL(returnPressed()), ui->toolButtonSend, SLOT(click()));
	toolButtonSmilies = new QToolButton();
	toolButtonSmilies->setPopupMode(QToolButton::InstantPopup);
	toolButtonSmilies->setToolTip(tr("Smilies"));
	toolButtonSmilies->setIcon(QIcon(":/Resource/Smileys/0.png"));
	widgetSmileyList = new WidgetSmileyList(this);
	toolButtonSmilies->setMenu(widgetSmileyList);
	toolButtonPickColor = new QToolButton(this);
	toolButtonPickColor->setStyleSheet(QString("QToolButton { background-color: %1; border-style: outset; border-width: 2px;	border-radius: 6px; border-color: lightgrey; }").arg(textEditInput->textColor().name()));
	toolButtonPickColor->setToolTip(tr("Font Color"));
	connect(toolButtonPickColor, SIGNAL(clicked()), this, SLOT(pickColor()));
	ui->toolBarTextTools->insertWidget(ui->actionBold, toolButtonPickColor);
	ui->toolBarTextTools->addSeparator();
	ui->toolBarTextTools->addWidget(toolButtonSmilies);
	ui->toolBarTextTools->addWidget(checkBoxSendOnEnter);
	ui->actionBold->setChecked(textEditInput->fontWeight() == QFont::Bold);
	ui->actionItalic->setChecked(textEditInput->fontItalic());
	ui->actionUnderline->setChecked(textEditInput->fontUnderline());
	connect(ui->actionItalic, SIGNAL(toggled(bool)), textEditInput, SLOT(setFontItalic(bool)));
	connect(ui->actionUnderline, SIGNAL(toggled(bool)), textEditInput, SLOT(setFontUnderline(bool)));
}

WidgetChatInput::~WidgetChatInput()
{
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
		emit messageSent(textEditInput->document());
		emit messageSent(textEditInput->document()->toPlainText());
		textEditInput->document()->clear();
	}
}

void WidgetChatInput::setText(QString text)
{
	textEditInput->setHtml(text);
}

void WidgetChatInput::pickColor()
{
	QColor fontColor;
	fontColor = QColorDialog::getColor(textEditInput->textColor(), this, tr("Select Font Color"));
	if (fontColor.isValid())
	{
		textEditInput->setTextColor(fontColor);
		toolButtonPickColor->setStyleSheet(QString("QToolButton { background-color: %1; border-style: outset; border-width: 2px;	border-radius: 6px; border-color: lightgrey; }").arg(fontColor.name()));
	}
}


void WidgetChatInput::pickSmiley()
{
	connect(this, SIGNAL(closing()), widgetSmileyList, SLOT(close()));
	widgetSmileyList->move(toolButtonSmilies->mapToGlobal(toolButtonSmilies->geometry().bottomLeft()));
	widgetSmileyList->show();
}

void WidgetChatInput::on_actionBold_toggled(bool checked)
{
	if (checked)
		textEditInput->setFontWeight(QFont::Bold);
	else
		textEditInput->setFontWeight(QFont::Normal);
}

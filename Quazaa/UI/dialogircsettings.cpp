#include "dialogircsettings.h"
#include "ui_dialogircsettings.h"

#include "quazaasettings.h"

DialogIrcSettings::DialogIrcSettings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogIrcSettings)
{
	ui->setupUi(this);

	setWindowFilePath(tr("Settings"));
	generalPage = new GeneralWizardPage(this);
	connect(generalPage, SIGNAL(settingsChanged()), this, SLOT(enableApply()));
	ui->tabWidgetSettings->addTab(generalPage, tr("General"));

	messagesPage = new MessagesWizardPage(this);
	connect(messagesPage, SIGNAL(settingsChanged()), this, SLOT(enableApply()));
	ui->tabWidgetSettings->addTab(messagesPage, tr("Messages"));

	colorsPage = new ColorsWizardPage(this);
	connect(colorsPage, SIGNAL(settingsChanged()), this, SLOT(enableApply()));
	ui->tabWidgetSettings->addTab(colorsPage, tr("Colors"));

	loadSettings();
	connect(ui->pushButtonApply, SIGNAL(clicked()), this, SLOT(saveSettings()));
	ui->pushButtonApply->setEnabled(false);
}

DialogIrcSettings::~DialogIrcSettings()
{
	delete ui;
}

void DialogIrcSettings::saveSettings()
{
/*	quazaaSettings.Chat.ConnectOnStartup = generalPage->connectOnStartup();
	quazaaSettings.Chat.MaxBlockCount = generalPage->maxBlockCount();
	quazaaSettings.Chat.TimeStamp = generalPage->timeStamp();
	quazaaSettings.Chat.Messages = messagesPage->messages();
	quazaaSettings.Chat.Highlights = messagesPage->highlights();
	quazaaSettings.Chat.Colors = colorsPage->colors(); */

	quazaaSettings.saveChat();

	ui->pushButtonApply->setEnabled(false);
}

void DialogIrcSettings::loadSettings()
{
/*	generalPage->setConnectOnStartup(quazaaSettings.Chat.ConnectOnStartup);
	generalPage->setMaxBlockCount(quazaaSettings.Chat.MaxBlockCount);
	generalPage->setTimeStamp(quazaaSettings.Chat.TimeStamp);
	messagesPage->setMessages(quazaaSettings.Chat.Messages);
	messagesPage->setHighlights(quazaaSettings.Chat.Highlights);
	colorsPage->setColors(quazaaSettings.Chat.Colors); */
}

void DialogIrcSettings::on_pushButtonOK_clicked()
{
	if(ui->pushButtonApply->isEnabled())
		saveSettings();
	accept();
}

void DialogIrcSettings::enableApply()
{
	ui->pushButtonApply->setEnabled(true);
}

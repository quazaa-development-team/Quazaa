/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "settingswizard.h"
#include "generalwizardpage.h"
#include "colorswizardpage.h"
#include "messageswizardpage.h"

SettingsWizard::SettingsWizard(QWidget* parent) : QWizard(parent)
{
    setWindowFilePath(tr("Settings"));
	setPage(GeneralPage, new GeneralWizardPage(this));
    setPage(MessagesPage, new MessagesWizardPage(this));
    setPage(ColorsPage, new ColorsWizardPage(this));
	loadSettings();
	connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
}

void SettingsWizard::saveSettings()
{
	quazaaSettings.Chat.ConnectOnStartup = static_cast<GeneralWizardPage*>(page(GeneralPage))->connectOnStartup();
	quazaaSettings.Chat.MaxBlockCount = static_cast<GeneralWizardPage*>(page(GeneralPage))->maxBlockCount();
	quazaaSettings.Chat.TimeStamp = static_cast<GeneralWizardPage*>(page(GeneralPage))->timeStamp();
	quazaaSettings.Chat.Messages = static_cast<MessagesWizardPage*>(page(MessagesPage))->messages();
	quazaaSettings.Chat.Highlights = static_cast<MessagesWizardPage*>(page(MessagesPage))->highlights();
	quazaaSettings.Chat.Colors = static_cast<ColorsWizardPage*>(page(ColorsPage))->colors();

	quazaaSettings.saveChat();
}

void SettingsWizard::loadSettings()
{
	static_cast<GeneralWizardPage*>(page(GeneralPage))->setConnectOnStartup(quazaaSettings.Chat.ConnectOnStartup);
	static_cast<GeneralWizardPage*>(page(GeneralPage))->setMaxBlockCount(quazaaSettings.Chat.MaxBlockCount);
	static_cast<GeneralWizardPage*>(page(GeneralPage))->setTimeStamp(quazaaSettings.Chat.TimeStamp);
	static_cast<MessagesWizardPage*>(page(MessagesPage))->setMessages(quazaaSettings.Chat.Messages);
	static_cast<MessagesWizardPage*>(page(MessagesPage))->setHighlights(quazaaSettings.Chat.Highlights);
	static_cast<ColorsWizardPage*>(page(ColorsPage))->setColors(quazaaSettings.Chat.Colors);
}

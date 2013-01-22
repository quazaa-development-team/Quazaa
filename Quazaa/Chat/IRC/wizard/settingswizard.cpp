/*
* Copyright (C) 2008-2013 J-P Nurmi <jpnurmi@gmail.com>
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
#include "shortcutswizardpage.h"
#include "colorswizardpage.h"

SettingsWizard::SettingsWizard(QWidget* parent) : QWizard(parent)
{
	setWindowTitle(tr("Settings"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	setPage(GeneralPage, new GeneralWizardPage(this));
	setPage(ShortcutsPage, new ShortcutsWizardPage(this));
	setPage(ColorsPage, new ColorsWizardPage(this));
	setOption(HaveCustomButton1, true);
	setButtonText(QWizard::CustomButton1, tr("Reset"));
	connect(button(QWizard::CustomButton1), SIGNAL(clicked()), this, SLOT(resetCurrentPage()));

	setOption(NoDefaultButton, false);
}

IrcSettings SettingsWizard::ircSettings() const
{
	IrcSettings ircSettings;
	ircSettings.font = static_cast<GeneralWizardPage*>(page(GeneralPage))->font().toString();
	ircSettings.layout = static_cast<GeneralWizardPage*>(page(GeneralPage))->layout();
	ircSettings.language = static_cast<GeneralWizardPage*>(page(GeneralPage))->language();
	ircSettings.maxBlockCount = static_cast<GeneralWizardPage*>(page(GeneralPage))->maxBlockCount();
	ircSettings.timeStamp = static_cast<GeneralWizardPage*>(page(GeneralPage))->timeStamp();
	ircSettings.stripNicks = static_cast<GeneralWizardPage*>(page(GeneralPage))->stripNicks();
	ircSettings.shortcuts = static_cast<ShortcutsWizardPage*>(page(ShortcutsPage))->shortcuts();
	ircSettings.colors = static_cast<ColorsWizardPage*>(page(ColorsPage))->colors();	return ircSettings;
}

void SettingsWizard::setIrcSettings(const IrcSettings& ircSettings)
{
	setGeneralSettings(ircSettings);
	setShortcutSettings(ircSettings);
	setColorSettings(ircSettings);
}

void SettingsWizard::setGeneralSettings(const IrcSettings& ircSettings)
{
	QFont font;
	if (!ircSettings.font.isEmpty())
		font.fromString(ircSettings.font);
	static_cast<GeneralWizardPage*>(page(GeneralPage))->setFont(font);
	static_cast<GeneralWizardPage*>(page(GeneralPage))->setLayout(ircSettings.layout);
	static_cast<GeneralWizardPage*>(page(GeneralPage))->setLanguage(ircSettings.language);
	static_cast<GeneralWizardPage*>(page(GeneralPage))->setMaxBlockCount(ircSettings.maxBlockCount);
	static_cast<GeneralWizardPage*>(page(GeneralPage))->setTimeStamp(ircSettings.timeStamp);
	static_cast<GeneralWizardPage*>(page(GeneralPage))->setStripNicks(ircSettings.stripNicks);
}

void SettingsWizard::setShortcutSettings(const IrcSettings& ircSettings)
{
	static_cast<ShortcutsWizardPage*>(page(ShortcutsPage))->setShortcuts(ircSettings.shortcuts);
}

void SettingsWizard::setColorSettings(const IrcSettings& ircSettings)
{
	static_cast<ColorsWizardPage*>(page(ColorsPage))->setColors(ircSettings.colors);
}

void SettingsWizard::resetCurrentPage()
{
	IrcSettings ircSettings; // default values
	switch (currentId()) {
		case GeneralPage: setGeneralSettings(ircSettings); break;
		case ShortcutsPage: setShortcutSettings(ircSettings); break;
		case ColorsPage: setColorSettings(ircSettings); break;		default: Q_ASSERT(false); break;
	}
}

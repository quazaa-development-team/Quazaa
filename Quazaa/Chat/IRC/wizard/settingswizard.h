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

#ifndef SETTINGSWIZARD_H
#define SETTINGSWIZARD_H

#include <QWizard>
#include "ircsettings.h"

class SettingsWizard : public QWizard
{
	Q_OBJECT

public:
	SettingsWizard(QWidget* parent = 0);

	enum Page {
		GeneralPage,
		ShortcutsPage,
		ColorsPage
	};
	IrcSettings ircSettings() const;
	void setIrcSettings(const IrcSettings& ircSettings);

protected:
	void setGeneralSettings(const IrcSettings& ircSettings);
	void setShortcutSettings(const IrcSettings& ircSettings);
	void setColorSettings(const IrcSettings& ircSettings);

private slots:
	void resetCurrentPage();
};

#endif // SETTINGSWIZARD_H

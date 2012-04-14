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

#include "serverwizardpage.h"
#include <QSettings>

ServerWizardPage::ServerWizardPage(QWidget* parent) : QWizardPage(parent)
{
	ui.setupUi(this);
	setPixmap(QWizard::LogoPixmap, QPixmap(":/Resource/oxygen/64x64/actions/network.png"));

    QSettings settings;
    QStringList hosts = settings.value("hosts").toStringList();
	if (!hosts.contains("irc.paradoxirc.net"))
		hosts.append("irc.paradoxirc.net");
	if (!hosts.contains("us.paradoxirc.net"))
		hosts.append("us.paradoxirc.net");
	if (!hosts.contains("eu.paradoxirc.net"))
		hosts.append("eu.paradoxirc.net");
	if (!hosts.contains("irc.p2pchat.net"))
		hosts.append("irc.p2pchat.net");
	if (!hosts.contains("irc.gigirc.net"))
		hosts.append("irc.gigirc.net");
	if (!hosts.contains("irc.freenode.net"))
		hosts.append("irc.freenode.net");
	if (!hosts.contains("irc.undernet.org"))
		hosts.append("irc.undernet.org");
	if (!hosts.contains("irc.quakenet.org"))
		hosts.append("irc.quakenet.org");
	if (!hosts.contains("irc.uni-erlangen.de"))
		hosts.append("irc.uni-erlangen.de");
	if (!hosts.contains("irc.servercentral.net"))
		hosts.append("irc.servercentral.net");
	if (!hosts.contains("irc.dal.net"))
		hosts.append("irc.dal.net");


}

bool ServerWizardPage::isComplete() const
{

}

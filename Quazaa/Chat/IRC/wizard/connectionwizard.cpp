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

#include "connectionwizard.h"
#include "quazaasettings.h"

ConnectionWizard::ConnectionWizard(QWidget* parent) : QWizard(parent)
{
    setWindowFilePath(tr("Connection"));
	quazaaSettings.loadChatConnectionWizard();
	pageUser = new UserWizardPage(this);
	pageServer = new ServerWizardPage(this);
	pageConnection = new ConnectionWizardPage(this);
	setPage(UserPage, pageUser);
	setPage(ServerPage, pageServer);
	setPage(ConnectionPage, pageConnection);
	setOption(NoDefaultButton, false);
}

void ConnectionWizard::accept()
{
	if (!quazaaSettings.Chat.NickNames.contains(pageUser->nickName(), Qt::CaseInsensitive))
		quazaaSettings.Chat.NickNames << pageUser->nickName();
	if (!quazaaSettings.Chat.RealNames.contains(pageUser->realName(), Qt::CaseInsensitive))
		quazaaSettings.Chat.RealNames << pageUser->realName();
	if (!quazaaSettings.Chat.Hosts.contains(pageServer->hostName(), Qt::CaseInsensitive))
		quazaaSettings.Chat.Hosts << pageServer->hostName();
	if (!quazaaSettings.Chat.ConnectionNames.contains(pageConnection->connectionName(), Qt::CaseInsensitive))
		quazaaSettings.Chat.ConnectionNames << pageConnection->connectionName();
	quazaaSettings.saveChatConnectionWizard();

	QDialog::accept();
}

ConnectionInfo ConnectionWizard::connection() const
{
	ConnectionInfo conn;
	conn.nick = pageUser->nickName();
	conn.real = pageUser->realName();
	conn.host = pageServer->hostName();
	conn.port = pageServer->port();
	conn.secure = pageServer->isSecure();
	conn.pass = pageServer->password();
	conn.name = pageConnection->connectionName();
    return conn;
}

void ConnectionWizard::setConnection(const ConnectionInfo& conn)
{
	pageUser->setNickName(conn.nick);
	pageUser->setRealName(conn.real);
	pageServer->setHostName(conn.host);
	pageServer->setPort(conn.port);
	pageServer->setSecure(conn.secure);
	pageServer->setPassword(conn.pass);
	pageConnection->setConnectionName(conn.name);
}

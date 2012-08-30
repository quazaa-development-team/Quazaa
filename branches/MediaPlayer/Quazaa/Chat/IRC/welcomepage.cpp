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

#include "welcomepage.h"
#include "settingswizard.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QFile>
#include <QApplication>

WelcomePage::WelcomePage(QWidget* parent) : QWidget(parent)
{
    header = new QLabel(this);
    header->setOpenExternalLinks(true);
    header->setWordWrap(true);
    footer = new QLabel(this);
    footer->setOpenExternalLinks(true);

    updateHtml();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(header);
    layout->addWidget(createBody(this), 1);
    layout->addWidget(footer);
    setLayout(layout);
}

static QString readHtmlFile(const QString& filePath)
{
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    return QString::fromUtf8(file.readAll());
}

void WelcomePage::updateHtml()
{
	QString headerHtml = readHtmlFile(":/Resource/welcome_header.html");
	headerHtml = headerHtml.arg(QApplication::applicationName())
			.arg(tr("Please respect others while using this service."));
    header->setText(headerHtml);
	footer->setText(readHtmlFile(":/Resource/welcome_footer.html"));
}

QWidget* WelcomePage::createBody(QWidget* parent)
{
    QWidget* body = new QWidget(parent);

	connectButton = new QCommandLinkButton(tr("Connect"), body);
	connectButton->setDescription(tr("Connect or create new IRC connection."));

    QGridLayout* layout = new QGridLayout(body);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(3, 1);
    layout->setRowStretch(0, 1);
	layout->addWidget(connectButton, 1, 1);

    layout->setRowStretch(6, 1);

	layout->setRowStretch(7, 1);
    body->setLayout(layout);

    return body;
}

void WelcomePage::showSettings()
{
	SettingsWizard wizard(qApp->activeWindow());
	wizard.exec();
}

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

#include "generalwizardpage.h"

GeneralWizardPage::GeneralWizardPage(QWidget* parent) : QWizardPage(parent)
{
    ui.setupUi(this);
	setPixmap(QWizard::LogoPixmap, QPixmap(":/Resource/oxygen/64x64/actions/configure.png"));
}

QString GeneralWizardPage::language() const
{
    // TODO
    return QString();
}

void GeneralWizardPage::setLanguage(const QString& language)
{
    // TODO
    Q_UNUSED(language);
}

bool GeneralWizardPage::connectOnStartup() const
{
	return ui.checkBoxConnectOnStartup->isChecked();
}

void GeneralWizardPage::setConnectOnStartup(bool connectOnStartup)
{
	ui.checkBoxConnectOnStartup->setChecked(connectOnStartup);
}

int GeneralWizardPage::maxBlockCount() const
{
    return ui.blockSpinBox->value();
}

void GeneralWizardPage::setMaxBlockCount(int count)
{
    ui.blockSpinBox->setValue(count);
}

bool GeneralWizardPage::timeStamp() const
{
    return ui.timeStampCheckBox->isChecked();
}

void GeneralWizardPage::setTimeStamp(bool timeStamp)
{
    ui.timeStampCheckBox->setChecked(timeStamp);
}

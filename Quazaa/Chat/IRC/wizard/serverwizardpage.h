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

#ifndef SERVERWIZARDPAGE_H
#define SERVERWIZARDPAGE_H

#include "ui_serverwizardpage.h"
#include "completerlineedit.h"

class ServerWizardPage : public QWizardPage
{
    Q_OBJECT

public:
	ServerWizardPage(QWidget* parent = 0);
	~ServerWizardPage();

private:
    Ui::ServerWizardPage ui;
};

#endif // SERVERWIZARDPAGE_H

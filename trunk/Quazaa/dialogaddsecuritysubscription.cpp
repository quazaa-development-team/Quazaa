//
// dialogaddsecuritysubscription.cpp
//
// Copyright © Quazaa Development Team, 2009.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "dialogaddsecuritysubscription.h"
#include "ui_dialogaddsecuritysubscription.h"

DialogAddSecuritySubscription::DialogAddSecuritySubscription(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DialogAddSecuritySubscription)
{
    m_ui->setupUi(this);
}

DialogAddSecuritySubscription::~DialogAddSecuritySubscription()
{
    delete m_ui;
}

void DialogAddSecuritySubscription::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DialogAddSecuritySubscription::on_pushButtonSubscribe_clicked()
{
	this->close();
}

void DialogAddSecuritySubscription::on_pushButtonCancel_clicked()
{
	this->close();
}

/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public 
** License version 3.0 requirements will be met: 
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version 
** 3.0 along with Quazaa; if not, write to the Free Software Foundation, 
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "dialogirccolordialog.h"
#include "ui_dialogirccolordialog.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

DialogIRCColorDialog::DialogIRCColorDialog(QColor initialColor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogIRCColorDialog)
{
    ui->setupUi(this);
	m_oIRCColor = initialColor;
}

DialogIRCColorDialog::~DialogIRCColorDialog()
{
    delete ui;
}

void DialogIRCColorDialog::on_toolButtonWhite_clicked()
{
	m_oIRCColor.setNamedColor("white");
	accept();
}

void DialogIRCColorDialog::on_toolButtonBlack_clicked()
{
	m_oIRCColor.setNamedColor("black");
	accept();
}

void DialogIRCColorDialog::on_toolButtonNavy_clicked()
{
	m_oIRCColor.setNamedColor("navy");
	accept();
}

void DialogIRCColorDialog::on_toolButtonGreen_clicked()
{
	m_oIRCColor.setNamedColor("green");
	accept();
}

void DialogIRCColorDialog::on_toolButtonGray_clicked()
{
	m_oIRCColor.setNamedColor("gray");
	accept();
}

void DialogIRCColorDialog::on_toolButtonLightGray_clicked()
{
	m_oIRCColor.setNamedColor("lightGray");
	accept();
}

void DialogIRCColorDialog::on_toolButtonBlue_clicked()
{
	m_oIRCColor.setNamedColor("blue");
	accept();
}

void DialogIRCColorDialog::on_toolButtonDarkCyan_clicked()
{
	m_oIRCColor.setNamedColor("darkCyan");
	accept();
}

void DialogIRCColorDialog::on_toolButtonCyan_clicked()
{
	m_oIRCColor.setNamedColor("cyan");
	accept();
}

void DialogIRCColorDialog::on_toolButtonPurple_clicked()
{
	m_oIRCColor.setNamedColor("purple");
	accept();
}

void DialogIRCColorDialog::on_toolButtonMagenta_clicked()
{
	m_oIRCColor.setNamedColor("magenta");
	accept();
}

void DialogIRCColorDialog::on_toolButtonLime_clicked()
{
	m_oIRCColor.setNamedColor("lime");
	accept();
}

void DialogIRCColorDialog::on_toolButtonRed_clicked()
{
	m_oIRCColor.setNamedColor("red");
	accept();
}

void DialogIRCColorDialog::on_toolButtonYellow_clicked()
{
	m_oIRCColor.setNamedColor("yellow");
	accept();
}

void DialogIRCColorDialog::on_toolButtonBrown_clicked()
{
	m_oIRCColor.setNamedColor("brown");
	accept();
}

void DialogIRCColorDialog::on_toolButtonCancel_clicked()
{
	reject();
}

void DialogIRCColorDialog::on_toolButtonOlive_clicked()
{
	m_oIRCColor.setNamedColor("olive");
	accept();
}


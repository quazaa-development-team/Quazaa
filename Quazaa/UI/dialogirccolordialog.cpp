/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2012.
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

DialogIrcColorDialog::DialogIrcColorDialog(QColor initialColor, QWidget *parent) :
    QDialog(parent),
	ui(new Ui::DialogIrcColorDialog)
{
    ui->setupUi(this);
	m_oIrcColor = initialColor;
	m_bDefault = false;
}

DialogIrcColorDialog::~DialogIrcColorDialog()
{
    delete ui;
}

void DialogIrcColorDialog::on_toolButtonCancel_clicked()
{
	reject();
}

bool DialogIrcColorDialog::isDefaultColor()
{
	return m_bDefault;
}

QColor DialogIrcColorDialog::color()
{
	return m_oIrcColor;
}

void DialogIrcColorDialog::on_toolButtonDefault_clicked()
{
	m_bDefault = true;
	accept();
}

void DialogIrcColorDialog::on_toolButtonDarkBlue_clicked()
{
	m_oIrcColor.setNamedColor("darkblue");
	accept();
}

void DialogIrcColorDialog::on_toolButtonDarkGreen_clicked()
{
	m_oIrcColor.setNamedColor("darkgreen");
	accept();
}

void DialogIrcColorDialog::on_toolButtonRed_clicked()
{
	m_oIrcColor.setNamedColor("red");
	accept();
}

void DialogIrcColorDialog::on_toolButtonDarkRed_clicked()
{
	m_oIrcColor.setNamedColor("darkred");
	accept();
}

void DialogIrcColorDialog::on_toolButtonDarkViolet_clicked()
{
	m_oIrcColor.setNamedColor("darkviolet");
	accept();
}

void DialogIrcColorDialog::on_toolButtonOrange_clicked()
{
	m_oIrcColor.setNamedColor("orange");
	accept();
}

void DialogIrcColorDialog::on_toolButtonYellow_clicked()
{
	m_oIrcColor.setNamedColor("yellow");
	accept();
}

void DialogIrcColorDialog::on_toolButtonLightGreen_clicked()
{
	m_oIrcColor.setNamedColor("lightgreen");
	accept();
}

void DialogIrcColorDialog::on_toolButtonCornFlowerBlue_clicked()
{
	m_oIrcColor.setNamedColor("cornflowerblue");
	accept();
}

void DialogIrcColorDialog::on_toolButtonLightBlue_clicked()
{
	m_oIrcColor.setNamedColor("lightblue");
	accept();
}

void DialogIrcColorDialog::on_toolButtonBlue_clicked()
{
	m_oIrcColor.setNamedColor("blue");
	accept();
}

void DialogIrcColorDialog::on_toolButtonViolet_clicked()
{
	m_oIrcColor.setNamedColor("violet");
	accept();
}

void DialogIrcColorDialog::on_toolButtonDarkGray_clicked()
{
	m_oIrcColor.setNamedColor("gray");
	accept();
}

void DialogIrcColorDialog::on_toolButtonGray_clicked()
{
	m_oIrcColor.setNamedColor("darkgray");
	accept();
}

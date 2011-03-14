/*
** dialoghashprogress.cpp
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

#include "dialoghashprogress.h"
#include "ui_dialoghashprogress.h"
 

DialogHashProgress::DialogHashProgress(QWidget* parent) :
	QDialog(parent),
	m_ui(new Ui::DialogHashProgress)
{
	m_ui->setupUi(this);
}

DialogHashProgress::~DialogHashProgress()
{
	delete m_ui;
}

void DialogHashProgress::changeEvent(QEvent* e)
{
	QDialog::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void DialogHashProgress::updateProgress(int percent, QString status, QString file)
{
	m_ui->progressBarStatus->setValue(percent);
	m_ui->labelStatus->setText(status);
	m_ui->labelFileName->setText(file);
}

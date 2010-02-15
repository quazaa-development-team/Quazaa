//
// dialoglanguage.cpp
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

#include "dialoglanguage.h"
#include "ui_dialoglanguage.h"
#include "quazaasettings.h"
#include <QFile>
#include <QFileInfo>
#include <QTranslator>


DialogLanguage::DialogLanguage(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DialogLanguage)
{
	m_ui->setupUi(this);
}

DialogLanguage::~DialogLanguage()
{
	delete m_ui;
}

void DialogLanguage::changeEvent(QEvent *e)
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

void DialogLanguage::on_pushButtonOK_clicked()
{
	switch (m_ui->listWidgetLanguages->currentRow())
	{
		case 0: //English
		{
			QFile languageFile(quazaaSettings.LanguageFile);
			QFileInfo languageFileInfo(languageFile);
			QTranslator *translator;
			quazaaSettings.LanguageDefault = true;
			if (languageFile.open(QIODevice::ReadOnly))
			{
				translator->load(languageFileInfo.fileName(), languageFileInfo.filePath());
				QApplication::removeTranslator(translator);
				quazaaSettings.LanguageFile = "";
			}
			break;
		}
		case 1:	//Afrikanns
			break;
		case 2:	//Arabic
			break;
		case 3:	//Català
			break;
		case 4:	//Chinese
			break;
		case 5:	//Ceština
			break;
		case 6: //Deutsch
			break;
		case 7: //Eesti
			break;
		case 8: //Español
			break;
		case 9: //Español Mexicano
			break;
		case 10: //Suomi
			break;
		case 11: //Français
			break;
		case 12: //????????
			break;
		case 13: //?????
			break;
		case 14: //Hrvatski
			break;
		case 15: //Magyar
			break;
		case 16: //Italian
			break;
		case 17: //Japanese
			break;
		case 18: //Lietuviu
			break;
		case 19: //Nederlands
			break;
		case 20: //Norsk
			break;
		case 21: //Polski
			break;
		case 22: //Português Brasileiro
			break;
		case 23: //P??????
			break;
		case 24: //Slovenšcina
			break;
		case 25: //Shqip
			break;
		case 26: //Srpski
			break;
		case 27: //Svenska
			break;
		case 28: //Türkçe
			break;
		case 29: //Thai
			break;
		default: //English
			QFile languageFile(quazaaSettings.LanguageFile);
			QFileInfo languageFileInfo(languageFile);
			QTranslator *translator;
			quazaaSettings.LanguageDefault = true;
			if (languageFile.open(QIODevice::ReadOnly))
			{
				translator->load(languageFileInfo.fileName(), languageFileInfo.filePath());
				QApplication::removeTranslator(translator);
				quazaaSettings.LanguageFile = "";
			}
			break;
	}
	quazaaSettings.saveLanguageSettings();
	emit closed();
	close();
}

void DialogLanguage::on_pushButtonCancel_clicked()
{
	emit closed();
	close();
}

void DialogLanguage::on_listWidgetLanguages_itemClicked(QListWidgetItem* item)
{
	m_ui->pushButtonOK->setEnabled(true);
}

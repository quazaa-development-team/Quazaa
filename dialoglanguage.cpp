//
// dialoglanguage.cpp
//
// Copyright © Quazaa Development Team, 2009-2010.
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
#include "quazaaglobals.h"
#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"
#include <QFile>
#include <QFileInfo>
#include <QTranslator>


DialogLanguage::DialogLanguage(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DialogLanguage)
{
	m_ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	//Set the list selection according to which language file is selected
	//English
	if (quazaaSettings.LanguageFile == ("quazaa_default_en"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(0);
	}
	//Afrikanns
	if (quazaaSettings.LanguageFile == ("quazaa_af"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(1);
	}
	//Arabic
	if (quazaaSettings.LanguageFile == ("quazaa_ar"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(2);
	}
	//Català
	if (quazaaSettings.LanguageFile == ("quazaa_ca"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(3);
	}
	//Chinese
	if (quazaaSettings.LanguageFile == ("quazaa_chs"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(4);
	}
	//Ceština
	if (quazaaSettings.LanguageFile == ("quazaa_cz"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(5);
	}
	//Deutsch
	if (quazaaSettings.LanguageFile == ("quazaa_de"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(6);
	}
	//Eesti
	if (quazaaSettings.LanguageFile == ("quazaa_ee"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(7);
	}
	//Español
	if (quazaaSettings.LanguageFile == ("quazaa_es"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(8);
	}
	//Español Mexicano
	if (quazaaSettings.LanguageFile == ("quazaa_es-mx"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(9);
	}
	//Suomi
	if (quazaaSettings.LanguageFile == ("quazaa_fi"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(10);
	}
	//Français
	if (quazaaSettings.LanguageFile == ("quazaa_fr"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(11);
	}
	//Greek
	if (quazaaSettings.LanguageFile == ("quazaa_gr"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(12);
	}
	//Hebrew
	if (quazaaSettings.LanguageFile == ("quazaa_heb"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(13);
	}
	//Hrvatski
	if (quazaaSettings.LanguageFile == ("quazaa_hr"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(14);
	}
	//Magyar
	if (quazaaSettings.LanguageFile == ("quazaa_hu"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(15);
	}
	//Italian
	if (quazaaSettings.LanguageFile == ("quazaa_it"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(16);
	}
	//Japanese
	if (quazaaSettings.LanguageFile == ("quazaa_ja"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(17);
	}
	//Lietuviu
	if (quazaaSettings.LanguageFile == ("quazaa_lt"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(18);
	}
	//Nederlands
	if (quazaaSettings.LanguageFile == ("quazaa_nl"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(19);
	}
	//Norsk
	if (quazaaSettings.LanguageFile == ("quazaa_no"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(20);
	}
	//Polski
	if (quazaaSettings.LanguageFile == ("quazaa_pl"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(21);
	}
	//Português Brasileiro
	if (quazaaSettings.LanguageFile == ("quazaa_pt-br"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(22);
	}
	//Russian
	if (quazaaSettings.LanguageFile == ("quazaa_ru"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(23);
	}
	//Slovenšcina
	if (quazaaSettings.LanguageFile == ("quazaa_sl-si"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(24);
	}
	//Shqip
	if (quazaaSettings.LanguageFile == ("quazaa_sq"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(25);
	}
	//Srpski
	if (quazaaSettings.LanguageFile == ("quazaa_sr"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(26);
	}
	//Svenska
	if (quazaaSettings.LanguageFile == ("quazaa_sv"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(27);
	}
	//Türkçe
	if (quazaaSettings.LanguageFile == ("quazaa_tr"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(28);
	}
	//Thai
	if (quazaaSettings.LanguageFile == ("quazaa_tw"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(29);
	}
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
			quazaaSettings.LanguageFile = ("quazaa_default_en");
			break;
		case 1:	//Afrikanns
			quazaaSettings.LanguageFile = ("quazaa_af");
			break;
		case 2:	//Arabic
			quazaaSettings.LanguageFile = ("quazaa_ar");
			break;
		case 3:	//Català
			quazaaSettings.LanguageFile = ("quazaa_ca");
			break;
		case 4:	//Chinese
			quazaaSettings.LanguageFile = ("quazaa_chs");
			break;
		case 5:	//Ceština
			quazaaSettings.LanguageFile = ("quazaa_cz");
			break;
		case 6: //Deutsch
			quazaaSettings.LanguageFile = ("quazaa_de");
			break;
		case 7: //Eesti
			quazaaSettings.LanguageFile = ("quazaa_ee");
			break;
		case 8: //Español
			quazaaSettings.LanguageFile = ("quazaa_es");
			break;
		case 9: //Español Mexicano
			quazaaSettings.LanguageFile = ("quazaa_es-mx");
			break;
		case 10: //Suomi
			quazaaSettings.LanguageFile = ("quazaa_fi");
			break;
		case 11: //Français
			quazaaSettings.LanguageFile = ("quazaa_fr");
			break;
		case 12: //Greek
			quazaaSettings.LanguageFile = ("quazaa_gr");
			break;
		case 13: //Hebrew
			quazaaSettings.LanguageFile = ("quazaa_heb");
			break;
		case 14: //Hrvatski
			quazaaSettings.LanguageFile = ("quazaa_hr");
			break;
		case 15: //Magyar
			quazaaSettings.LanguageFile = ("quazaa_hu");
			break;
		case 16: //Italian
			quazaaSettings.LanguageFile = ("quazaa_it");
			break;
		case 17: //Japanese
			quazaaSettings.LanguageFile = ("quazaa_ja");
			break;
		case 18: //Lietuviu
			quazaaSettings.LanguageFile = ("quazaa_lt");
			break;
		case 19: //Nederlands
			quazaaSettings.LanguageFile = ("quazaa_nl");
			break;
		case 20: //Norsk
			quazaaSettings.LanguageFile = ("quazaa_no");
			break;
		case 21: //Polski
			quazaaSettings.LanguageFile = ("quazaa_pl");
			break;
		case 22: //Português Brasileiro
			quazaaSettings.LanguageFile = ("quazaa_pt-br");
			break;
		case 23: //Russian
			quazaaSettings.LanguageFile = ("quazaa_ru");
			break;
		case 24: //Slovenšcina
			quazaaSettings.LanguageFile = ("quazaa_sl-si");
			break;
		case 25: //Shqip
			quazaaSettings.LanguageFile = ("quazaa_sq");
			break;
		case 26: //Srpski
			quazaaSettings.LanguageFile = ("quazaa_sr");
			break;
		case 27: //Svenska
			quazaaSettings.LanguageFile = ("quazaa_sv");
			break;
		case 28: //Türkçe
			quazaaSettings.LanguageFile = ("quazaa_tr");
			break;
		case 29: //Thai
			quazaaSettings.LanguageFile = ("quazaa_tw");
			break;
		default: //English
			quazaaSettings.LanguageFile = ("quazaa_default_en");
			break;
	}
	quazaaGlobals.translator.load(quazaaSettings.LanguageFile);
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

void DialogLanguage::skinChangeEvent()
{
	m_ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
}

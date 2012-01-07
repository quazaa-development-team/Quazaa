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

#include "dialoglanguage.h"
#include "ui_dialoglanguage.h"
#include "quazaaglobals.h"
#include "quazaasettings.h"
#include <QFile>
#include <QFileInfo>
#include <QTranslator>

#ifdef _DEBUG
#include "debug_new.h"
#endif

DialogLanguage::DialogLanguage(QWidget* parent) :
	QDialog(parent),
	m_ui(new Ui::DialogLanguage)
{
	m_ui->setupUi(this);
	//Set the list selection according to which language file is selected
	//English
	if(quazaaSettings.Language.File == ("quazaa_default_en"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(0);
	}
	//Afrikanns
	if(quazaaSettings.Language.File == ("quazaa_af"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(1);
	}
	//Arabic
	if(quazaaSettings.Language.File == ("quazaa_ar"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(2);
	}
	//Catal�
	if(quazaaSettings.Language.File == ("quazaa_ca"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(3);
	}
	//Chinese
	if(quazaaSettings.Language.File == ("quazaa_chs"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(4);
	}
	//Ce�tina
	if(quazaaSettings.Language.File == ("quazaa_cz"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(5);
	}
	//Deutsch
	if(quazaaSettings.Language.File == ("quazaa_de"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(6);
	}
	//Eesti
	if(quazaaSettings.Language.File == ("quazaa_ee"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(7);
	}
	//Espa�ol
	if(quazaaSettings.Language.File == ("quazaa_es"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(8);
	}
	//Farsi
	if(quazaaSettings.Language.File == ("quazaa_fa"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(9);
	}
	//Suomi
	if(quazaaSettings.Language.File == ("quazaa_fi"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(10);
	}
	//Fran�ais
	if(quazaaSettings.Language.File == ("quazaa_fr"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(11);
	}
	//Greek
	if(quazaaSettings.Language.File == ("quazaa_gr"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(12);
	}
	//Hebrew
	if(quazaaSettings.Language.File == ("quazaa_heb"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(13);
	}
	//Hrvatski
	if(quazaaSettings.Language.File == ("quazaa_hr"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(14);
	}
	//Magyar
	if(quazaaSettings.Language.File == ("quazaa_hu"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(15);
	}
	//Italian
	if(quazaaSettings.Language.File == ("quazaa_it"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(16);
	}
	//Japanese
	if(quazaaSettings.Language.File == ("quazaa_ja"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(17);
	}
	//Lietuviu
	if(quazaaSettings.Language.File == ("quazaa_lt"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(18);
	}
	//Nederlands
	if(quazaaSettings.Language.File == ("quazaa_nl"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(19);
	}
	//Norsk
	if(quazaaSettings.Language.File == ("quazaa_no"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(20);
	}
	//Polski
	if(quazaaSettings.Language.File == ("quazaa_pl"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(21);
	}
	//Portugu�s Brasileiro
	if(quazaaSettings.Language.File == ("quazaa_pt-br"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(22);
	}
	//Russian
	if(quazaaSettings.Language.File == ("quazaa_ru"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(23);
	}
	//Sloven�cina
	if(quazaaSettings.Language.File == ("quazaa_sl-si"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(24);
	}
	//Shqip
	if(quazaaSettings.Language.File == ("quazaa_sq"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(25);
	}
	//Srpski
	if(quazaaSettings.Language.File == ("quazaa_sr"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(26);
	}
	//Svenska
	if(quazaaSettings.Language.File == ("quazaa_sv"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(27);
	}
	//T�rk�e
	if(quazaaSettings.Language.File == ("quazaa_tr"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(28);
	}
	//Thai
	if(quazaaSettings.Language.File == ("quazaa_tw"))
	{
		m_ui->listWidgetLanguages->setCurrentRow(29);
	}
}

DialogLanguage::~DialogLanguage()
{
	delete m_ui;
}

void DialogLanguage::changeEvent(QEvent* e)
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

void DialogLanguage::on_pushButtonOK_clicked()
{
	switch(m_ui->listWidgetLanguages->currentRow())
	{
		case 0: //English
			quazaaSettings.Language.File = ("quazaa_default_en");
			break;
		case 1:	//Afrikanns
			quazaaSettings.Language.File = ("quazaa_af");
			break;
		case 2:	//Arabic
			quazaaSettings.Language.File = ("quazaa_ar");
			break;
		case 3:	//Catal�
			quazaaSettings.Language.File = ("quazaa_ca");
			break;
		case 4:	//Chinese
			quazaaSettings.Language.File = ("quazaa_chs");
			break;
		case 5:	//Ce�tina
			quazaaSettings.Language.File = ("quazaa_cz");
			break;
		case 6: //Deutsch
			quazaaSettings.Language.File = ("quazaa_de");
			break;
		case 7: //Eesti
			quazaaSettings.Language.File = ("quazaa_ee");
			break;
		case 8: //Espa�ol
			quazaaSettings.Language.File = ("quazaa_es");
			break;
		case 9: //Farsi
			quazaaSettings.Language.File = ("quazaa_fa");
			break;
		case 10: //Suomi
			quazaaSettings.Language.File = ("quazaa_fi");
			break;
		case 11: //Fran�ais
			quazaaSettings.Language.File = ("quazaa_fr");
			break;
		case 12: //Greek
			quazaaSettings.Language.File = ("quazaa_gr");
			break;
		case 13: //Hebrew
			quazaaSettings.Language.File = ("quazaa_heb");
			break;
		case 14: //Hrvatski
			quazaaSettings.Language.File = ("quazaa_hr");
			break;
		case 15: //Magyar
			quazaaSettings.Language.File = ("quazaa_hu");
			break;
		case 16: //Italian
			quazaaSettings.Language.File = ("quazaa_it");
			break;
		case 17: //Japanese
			quazaaSettings.Language.File = ("quazaa_ja");
			break;
		case 18: //Lietuviu
			quazaaSettings.Language.File = ("quazaa_lt");
			break;
		case 19: //Nederlands
			quazaaSettings.Language.File = ("quazaa_nl");
			break;
		case 20: //Norsk
			quazaaSettings.Language.File = ("quazaa_no");
			break;
		case 21: //Polski
			quazaaSettings.Language.File = ("quazaa_pl");
			break;
		case 22: //Portugu�s Brasileiro
			quazaaSettings.Language.File = ("quazaa_pt-br");
			break;
		case 23: //Russian
			quazaaSettings.Language.File = ("quazaa_ru");
			break;
		case 24: //Sloven�cina
			quazaaSettings.Language.File = ("quazaa_sl-si");
			break;
		case 25: //Shqip
			quazaaSettings.Language.File = ("quazaa_sq");
			break;
		case 26: //Srpski
			quazaaSettings.Language.File = ("quazaa_sr");
			break;
		case 27: //Svenska
			quazaaSettings.Language.File = ("quazaa_sv");
			break;
		case 28: //T�rk�e
			quazaaSettings.Language.File = ("quazaa_tr");
			break;
		case 29: //Thai
			quazaaSettings.Language.File = ("quazaa_tw");
			break;
		default: //English
			quazaaSettings.Language.File = ("quazaa_default_en");
			break;
	}
	quazaaSettings.translator.load(quazaaSettings.Language.File);
	quazaaSettings.saveLanguageSettings();
	close();
}

void DialogLanguage::on_pushButtonCancel_clicked()
{
	emit closed();
	close();
}

void DialogLanguage::on_listWidgetLanguages_itemClicked(QListWidgetItem* item)
{
	Q_UNUSED(item);

	m_ui->pushButtonOK->setEnabled(true);
}


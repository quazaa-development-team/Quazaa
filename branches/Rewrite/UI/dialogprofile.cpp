/*
** dialogprofile.cpp
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

#include "dialogprofile.h"
#include "ui_dialogprofile.h"
#include "quazaasettings.h"
#include <QFileDialog>
#include <QUuid>

DialogProfile::DialogProfile(QWidget* parent) :
	QDialog(parent),
	m_ui(new Ui::DialogProfile)
{
	m_ui->setupUi(this);
	m_ui->pagesProfile->setCurrentIndex(0);
	m_ui->lineEditGnutellaSceenName->setText(quazaaSettings.Profile.GnutellaScreenName);
	m_ui->lineEditNickname->setText(quazaaSettings.Profile.IrcNickname);
	m_ui->lineEditAltNickname->setText(quazaaSettings.Profile.IrcAlternateNickname);
	m_ui->lineEditUserName->setText(quazaaSettings.Profile.IrcUserName);
	m_ui->lineEditRealName->setText(quazaaSettings.Profile.RealName);
	m_ui->comboBoxGender->setCurrentIndex(quazaaSettings.Profile.Gender);
	m_ui->spinBoxAge->setValue(quazaaSettings.Profile.Age);
	m_ui->checkBoxAllowBrowse->setChecked(quazaaSettings.Security.AllowProfileBrowse);
	m_ui->lineEditEmail->setText(quazaaSettings.Profile.Email);
	m_ui->lineEditMSN->setText(quazaaSettings.Profile.MSNPassport);
	m_ui->lineEditYahoo->setText(quazaaSettings.Profile.YahooID);
	m_ui->lineEditICQ->setText(quazaaSettings.Profile.ICQuin);
	m_ui->lineEditAOL->setText(quazaaSettings.Profile.AolScreenName);
	m_ui->lineEditJabber->setText(quazaaSettings.Profile.JabberID);
	m_ui->lineEditMySpace->setText(quazaaSettings.Profile.MyspaceProfile);
	m_ui->lineEditCountry->setText(quazaaSettings.Profile.Country);
	m_ui->lineEditStateProvince->setText(quazaaSettings.Profile.StateProvince);
	m_ui->lineEditCity->setText(quazaaSettings.Profile.City);
	m_ui->lineEditLatitude->setText(quazaaSettings.Profile.Latitude);
	m_ui->lineEditLonitude->setText(quazaaSettings.Profile.Longitude);
	m_ui->listWidgetInterests->addItems(quazaaSettings.Profile.Interests);
	m_ui->plainTextEditBio->setPlainText(quazaaSettings.Profile.Biography);
	if(quazaaSettings.Profile.AvatarPath == ":/Chat/Graphics/Chat/DefaultAvatar.png")
	{
		m_sTempAvatarFileName = quazaaSettings.Profile.AvatarPath;
		m_ui->labelAvatarPreview->setPixmap(m_sTempAvatarFileName);
	}
	else
	{
		QFile avatarExists;
		avatarExists.setFileName(quazaaSettings.Profile.AvatarPath);
		if(avatarExists.exists(quazaaSettings.Profile.AvatarPath))
		{
			m_sTempAvatarFileName = quazaaSettings.Profile.AvatarPath;
			m_ui->labelAvatarPreview->setPixmap(m_sTempAvatarFileName);
		}
		else
		{
			quazaaSettings.Profile.AvatarPath = ":/Resource/Chat/DefaultAvatar.png";
			m_sTempAvatarFileName = quazaaSettings.Profile.AvatarPath;
			m_ui->labelAvatarPreview->setPixmap(m_sTempAvatarFileName);
		}
	}
	for(int m_iFavoritesIndex = 0; m_iFavoritesIndex < quazaaSettings.Profile.Favorites.size(); ++m_iFavoritesIndex)
	{
		QTreeWidgetItem* m_qTreeWidgetItem = new QTreeWidgetItem();
		m_qTreeWidgetItem->setText(0, quazaaSettings.Profile.Favorites.at(m_iFavoritesIndex));
		m_qTreeWidgetItem->setText(1, quazaaSettings.Profile.FavoritesURL.at(m_iFavoritesIndex));
		m_ui->treeWidgetFavorites->addTopLevelItem(m_qTreeWidgetItem);
	}
	tempGUID = QUuid::createUuid();
	m_ui->labelGUID->setText(tempGUID.toString().remove(QRegExp("[{}]")));
	m_ui->pushButtonApply->setEnabled(false);
}

DialogProfile::~DialogProfile()
{
	delete m_ui;
}

void DialogProfile::changeEvent(QEvent* e)
{
	switch(e->type())
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void DialogProfile::on_treeWidgetProfileNavigation_itemSelectionChanged()
{
	QTreeWidgetItem* item = m_ui->treeWidgetProfileNavigation->currentItem();
	int column = m_ui->treeWidgetProfileNavigation->currentColumn();
	QString m_sItemName;
	m_sItemName = item->text(column);

	if(m_sItemName == tr("Identity"))
	{
		m_ui->pagesProfile->setCurrentIndex(0);
		return;
	}
	else if(m_sItemName == tr("Contact"))
	{
		m_ui->pagesProfile->setCurrentIndex(1);
		return;
	}
	else if(m_sItemName == tr("Profile"))
	{
		m_ui->pagesProfile->setCurrentIndex(2);
		return;
	}
	else if(m_sItemName == tr("Bio"))
	{
		m_ui->pagesProfile->setCurrentIndex(3);
		return;
	}
	else if(m_sItemName == tr("Avatar"))
	{
		m_ui->pagesProfile->setCurrentIndex(4);
		return;
	}
	else if(m_sItemName == tr("Favorites"))
	{
		m_ui->pagesProfile->setCurrentIndex(5);
		return;
	}
	else if(m_sItemName == tr("Files"))
	{
		m_ui->pagesProfile->setCurrentIndex(6);
		return;
	}
	else if(m_sItemName == tr("Certificate"))
	{
		m_ui->pagesProfile->setCurrentIndex(7);
		return;
	}
}

void DialogProfile::on_pushButtonOK_clicked()
{
	if(m_ui->pushButtonApply->isEnabled())
	{
		m_ui->pushButtonApply->click();
	}
	emit closed();
	close();
}

void DialogProfile::on_pushButtonCancel_clicked()
{
	emit closed();
	close();
}

void DialogProfile::on_lineEditGnutellaSceenName_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_pushButtonApply_clicked()
{
	quazaaSettings.Profile.GnutellaScreenName = m_ui->lineEditGnutellaSceenName->text();
	quazaaSettings.Profile.IrcNickname = m_ui->lineEditNickname->text();
	quazaaSettings.Profile.IrcAlternateNickname = m_ui->lineEditAltNickname->text();
	quazaaSettings.Profile.IrcUserName = m_ui->lineEditUserName->text();
	quazaaSettings.Profile.RealName = m_ui->lineEditRealName->text();
	quazaaSettings.Profile.Gender = m_ui->comboBoxGender->currentIndex();
	quazaaSettings.Profile.Age = m_ui->spinBoxAge->value();
	quazaaSettings.Security.AllowProfileBrowse = m_ui->checkBoxAllowBrowse->isChecked();
	quazaaSettings.Profile.Email = m_ui->lineEditEmail->text();
	quazaaSettings.Profile.MSNPassport = m_ui->lineEditMSN->text();
	quazaaSettings.Profile.YahooID = m_ui->lineEditYahoo->text();
	quazaaSettings.Profile.ICQuin = m_ui->lineEditICQ->text();
	quazaaSettings.Profile.AolScreenName = m_ui->lineEditAOL->text();
	quazaaSettings.Profile.JabberID = m_ui->lineEditJabber->text();
	quazaaSettings.Profile.MyspaceProfile = m_ui->lineEditMySpace->text();
	quazaaSettings.Profile.Country = m_ui->lineEditCountry->text();
	quazaaSettings.Profile.StateProvince = m_ui->lineEditStateProvince->text();
	quazaaSettings.Profile.City = m_ui->lineEditCity->text();
	quazaaSettings.Profile.Interests.clear();
	for(int m_iInterestsRow = 0; m_iInterestsRow < m_ui->listWidgetInterests->count(); m_iInterestsRow++)
	{
		m_ui->listWidgetInterests->setCurrentRow(m_iInterestsRow);
		quazaaSettings.Profile.Interests.append(m_ui->listWidgetInterests->currentItem()->text());
	}
	quazaaSettings.Profile.Interests.removeDuplicates();
	m_ui->listWidgetInterests->clear();
	m_ui->listWidgetInterests->addItems(quazaaSettings.Profile.Interests);
	quazaaSettings.Profile.Biography = m_ui->plainTextEditBio->toPlainText();
	quazaaSettings.Profile.AvatarPath = m_sTempAvatarFileName;
	quazaaSettings.Profile.Favorites.clear();
	quazaaSettings.Profile.FavoritesURL.clear();
	for(int m_iFavorites = 0; m_iFavorites < m_ui->treeWidgetFavorites->topLevelItemCount(); ++m_iFavorites)
	{
		m_ui->treeWidgetFavorites->setCurrentItem(m_ui->treeWidgetFavorites->topLevelItem(m_iFavorites));
		quazaaSettings.Profile.Favorites.append(m_ui->treeWidgetFavorites->currentItem()->text(0));
		quazaaSettings.Profile.FavoritesURL.append(m_ui->treeWidgetFavorites->currentItem()->text(1));
	}
	quazaaSettings.Profile.GUID = tempGUID;
	quazaaSettings.saveProfile();
	m_ui->pushButtonApply->setEnabled(false);
}

void DialogProfile::on_lineEditNickname_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditAltNickname_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditUserName_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditRealName_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_comboBoxGender_currentIndexChanged(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_spinBoxAge_valueChanged(int)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_checkBoxAllowBrowse_clicked()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditEmail_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditMSN_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditYahoo_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditICQ_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditAOL_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditJabber_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditMySpace_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditCountry_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditStateProvince_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditCity_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditLatitude_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditLonitude_textEdited(QString)
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditInterest_textEdited(QString)
{
	if(m_ui->lineEditInterest->text() == "")
	{
		m_ui->pushButtonInterestsAdd->setEnabled(false);
	}
	else
	{
		m_ui->pushButtonInterestsAdd->setEnabled(true);
	}
}

void DialogProfile::on_pushButtonInterestsAdd_clicked()
{
	if(m_ui->lineEditInterest->text() != "")
	{
		m_ui->listWidgetInterests->addItem(m_ui->lineEditInterest->text());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogProfile::on_pushButtonInterestsRemove_clicked()
{
	if(m_ui->listWidgetInterests->currentRow() != -1)
	{
		m_ui->listWidgetRemoved->addItem(m_ui->listWidgetInterests->currentItem()->text());
		m_ui->listWidgetInterests->takeItem(m_ui->listWidgetInterests->currentRow());
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogProfile::on_listWidgetRemoved_itemClicked(QListWidgetItem* item)
{
	m_ui->lineEditInterest->setText(item->text());
	m_ui->pushButtonInterestsAdd->setEnabled(true);
}

void DialogProfile::on_plainTextEditBio_textChanged()
{
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_pushButtonAvatarAdd_clicked()
{
	m_sTempAvatarFileName = QFileDialog::getOpenFileName(this, tr("Open Avatar Image"),
	                        qApp->applicationDirPath(),
	                        tr("Images") + " (*.bmp *.png *.xbm *.xpm *.jpg *.jpeg *.gif *.pbm *.pgm *.ppm)");
	if(m_sTempAvatarFileName != "")
	{
		m_ui->labelAvatarPreview->setPixmap(m_sTempAvatarFileName);
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogProfile::on_pushButtonRemove_clicked()
{
	m_sTempAvatarFileName = ":/Chat/Graphics/Chat/DefaultAvatar.png";
	m_ui->labelAvatarPreview->setPixmap(QPixmap(m_sTempAvatarFileName));
	m_ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_pushButtonNewWebsite_clicked()
{
	if(m_ui->lineEditAddress->text() != "" && m_ui->lineEditWebsite->text() != "")
	{
		QTreeWidgetItem* m_qTreeWidgetItem = new QTreeWidgetItem();
		m_qTreeWidgetItem->setText(0, m_ui->lineEditWebsite->text());
		m_qTreeWidgetItem->setText(1, m_ui->lineEditAddress->text());
		m_ui->treeWidgetFavorites->addTopLevelItem(m_qTreeWidgetItem);
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogProfile::on_pushButtonRemoveWebsite_clicked()
{
	if(m_ui->treeWidgetFavorites->currentIndex().isValid())
	{
		m_ui->treeWidgetFavorites->takeTopLevelItem(m_ui->treeWidgetFavorites->indexOfTopLevelItem
		        (m_ui->treeWidgetFavorites->currentItem()));
		m_ui->pushButtonApply->setEnabled(true);
	}
}

void DialogProfile::on_pushButtonGenerateNewGUID_clicked()
{
	tempGUID = QUuid::createUuid();
	m_ui->labelGUID->setText(tempGUID.toString().remove(QRegExp("[{}]")));
	m_ui->pushButtonApply->setEnabled(true);
}

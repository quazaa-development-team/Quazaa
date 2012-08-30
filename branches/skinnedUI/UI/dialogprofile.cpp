//
// dialogprofile.cpp
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

#include "dialogprofile.h"
#include "ui_dialogprofile.h"
#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"
#include <QFileDialog>
#include <QUuid>

DialogProfile::DialogProfile(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogProfile)
{
	ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	ui->pagesProfile->setCurrentIndex(0);
	ui->lineEditGnutellaSceenName->setText(quazaaSettings.Profile.GnutellaScreenName);
	ui->lineEditNickname->setText(quazaaSettings.Profile.IrcNickname);
	ui->lineEditAltNickname->setText(quazaaSettings.Profile.IrcAlternateNickname);
	ui->lineEditUserName->setText(quazaaSettings.Profile.IrcUserName);
	ui->lineEditRealName->setText(quazaaSettings.Profile.RealName);
	ui->comboBoxGender->setCurrentIndex(quazaaSettings.Profile.Gender);
	ui->spinBoxAge->setValue(quazaaSettings.Profile.Age);
	ui->checkBoxAllowBrowse->setChecked(quazaaSettings.Security.AllowProfileBrowse);
	ui->lineEditEmail->setText(quazaaSettings.Profile.Email);
	ui->lineEditMSN->setText(quazaaSettings.Profile.MSNPassport);
	ui->lineEditYahoo->setText(quazaaSettings.Profile.YahooID);
	ui->lineEditICQ->setText(quazaaSettings.Profile.ICQuin);
	ui->lineEditAOL->setText(quazaaSettings.Profile.AolScreenName);
	ui->lineEditJabber->setText(quazaaSettings.Profile.JabberID);
	ui->lineEditMySpace->setText(quazaaSettings.Profile.MyspaceProfile);
	ui->lineEditCountry->setText(quazaaSettings.Profile.Country);
	ui->lineEditStateProvince->setText(quazaaSettings.Profile.StateProvince);
	ui->lineEditCity->setText(quazaaSettings.Profile.City);
	ui->lineEditLatitude->setText(quazaaSettings.Profile.Latitude);
	ui->lineEditLonitude->setText(quazaaSettings.Profile.Longitude);
	ui->listWidgetInterests->addItems(quazaaSettings.Profile.Interests);
	ui->plainTextEditBio->setPlainText(quazaaSettings.Profile.Biography);
	if(quazaaSettings.Profile.AvatarPath == ":/Chat/Graphics/Chat/DefaultAvatar.png")
	{
		m_sTempAvatarFileName = quazaaSettings.Profile.AvatarPath;
		ui->labelAvatarPreview->setPixmap(m_sTempAvatarFileName);
	}
	else
	{
		QFile avatarExists;
		avatarExists.setFileName(quazaaSettings.Profile.AvatarPath);
		if(avatarExists.exists(quazaaSettings.Profile.AvatarPath))
		{
			m_sTempAvatarFileName = quazaaSettings.Profile.AvatarPath;
			ui->labelAvatarPreview->setPixmap(m_sTempAvatarFileName);
		}
		else
		{
			quazaaSettings.Profile.AvatarPath = ":/Resource/Chat/DefaultAvatar.png";
			m_sTempAvatarFileName = quazaaSettings.Profile.AvatarPath;
			ui->labelAvatarPreview->setPixmap(m_sTempAvatarFileName);
		}
	}
	for(int m_iFavoritesIndex = 0; m_iFavoritesIndex < quazaaSettings.Profile.Favorites.size(); ++m_iFavoritesIndex)
	{
		QTreeWidgetItem* m_qTreeWidgetItem = new QTreeWidgetItem();
		m_qTreeWidgetItem->setText(0, quazaaSettings.Profile.Favorites.at(m_iFavoritesIndex));
		m_qTreeWidgetItem->setText(1, quazaaSettings.Profile.FavoritesURL.at(m_iFavoritesIndex));
		ui->treeWidgetFavorites->addTopLevelItem(m_qTreeWidgetItem);
	}
	tempGUID = QUuid::createUuid();
	ui->labelGUID->setText(tempGUID.toString().remove(QRegExp("[{}]")));
	ui->pushButtonApply->setEnabled(false);
}

DialogProfile::~DialogProfile()
{
	delete ui;
}

void DialogProfile::changeEvent(QEvent* e)
{
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void DialogProfile::on_treeWidgetProfileNavigation_itemSelectionChanged()
{
	QTreeWidgetItem* item = ui->treeWidgetProfileNavigation->currentItem();
	int column = ui->treeWidgetProfileNavigation->currentColumn();
	QString m_sItemName;
	m_sItemName = item->text(column);

	if(m_sItemName == tr("Identity"))
	{
		ui->pagesProfile->setCurrentIndex(0);
		return;
	}
	else if(m_sItemName == tr("Contact"))
	{
		ui->pagesProfile->setCurrentIndex(1);
		return;
	}
	else if(m_sItemName == tr("Profile"))
	{
		ui->pagesProfile->setCurrentIndex(2);
		return;
	}
	else if(m_sItemName == tr("Bio"))
	{
		ui->pagesProfile->setCurrentIndex(3);
		return;
	}
	else if(m_sItemName == tr("Avatar"))
	{
		ui->pagesProfile->setCurrentIndex(4);
		return;
	}
	else if(m_sItemName == tr("Favorites"))
	{
		ui->pagesProfile->setCurrentIndex(5);
		return;
	}
	else if(m_sItemName == tr("Files"))
	{
		ui->pagesProfile->setCurrentIndex(6);
		return;
	}
	else if(m_sItemName == tr("Certificate"))
	{
		ui->pagesProfile->setCurrentIndex(7);
		return;
	}
}

void DialogProfile::on_pushButtonOK_clicked()
{
	if(ui->pushButtonApply->isEnabled())
	{
		ui->pushButtonApply->click();
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
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_pushButtonApply_clicked()
{
	quazaaSettings.Profile.GnutellaScreenName = ui->lineEditGnutellaSceenName->text();
	quazaaSettings.Profile.IrcNickname = ui->lineEditNickname->text();
	quazaaSettings.Profile.IrcAlternateNickname = ui->lineEditAltNickname->text();
	quazaaSettings.Profile.IrcUserName = ui->lineEditUserName->text();
	quazaaSettings.Profile.RealName = ui->lineEditRealName->text();
	quazaaSettings.Profile.Gender = ui->comboBoxGender->currentIndex();
	quazaaSettings.Profile.Age = ui->spinBoxAge->value();
	quazaaSettings.Security.AllowProfileBrowse = ui->checkBoxAllowBrowse->isChecked();
	quazaaSettings.Profile.Email = ui->lineEditEmail->text();
	quazaaSettings.Profile.MSNPassport = ui->lineEditMSN->text();
	quazaaSettings.Profile.YahooID = ui->lineEditYahoo->text();
	quazaaSettings.Profile.ICQuin = ui->lineEditICQ->text();
	quazaaSettings.Profile.AolScreenName = ui->lineEditAOL->text();
	quazaaSettings.Profile.JabberID = ui->lineEditJabber->text();
	quazaaSettings.Profile.MyspaceProfile = ui->lineEditMySpace->text();
	quazaaSettings.Profile.Country = ui->lineEditCountry->text();
	quazaaSettings.Profile.StateProvince = ui->lineEditStateProvince->text();
	quazaaSettings.Profile.City = ui->lineEditCity->text();
	quazaaSettings.Profile.Interests.clear();
	for(int m_iInterestsRow = 0; m_iInterestsRow < ui->listWidgetInterests->count(); m_iInterestsRow++)
	{
		ui->listWidgetInterests->setCurrentRow(m_iInterestsRow);
		quazaaSettings.Profile.Interests.append(ui->listWidgetInterests->currentItem()->text());
	}
	quazaaSettings.Profile.Interests.removeDuplicates();
	ui->listWidgetInterests->clear();
	ui->listWidgetInterests->addItems(quazaaSettings.Profile.Interests);
	quazaaSettings.Profile.Biography = ui->plainTextEditBio->toPlainText();
	quazaaSettings.Profile.AvatarPath = m_sTempAvatarFileName;
	quazaaSettings.Profile.Favorites.clear();
	quazaaSettings.Profile.FavoritesURL.clear();
	for(int m_iFavorites = 0; m_iFavorites < ui->treeWidgetFavorites->topLevelItemCount(); ++m_iFavorites)
	{
		ui->treeWidgetFavorites->setCurrentItem(ui->treeWidgetFavorites->topLevelItem(m_iFavorites));
		quazaaSettings.Profile.Favorites.append(ui->treeWidgetFavorites->currentItem()->text(0));
		quazaaSettings.Profile.FavoritesURL.append(ui->treeWidgetFavorites->currentItem()->text(1));
	}
	quazaaSettings.Profile.GUID = tempGUID;
	quazaaSettings.saveProfile();
	ui->pushButtonApply->setEnabled(false);
}

void DialogProfile::on_lineEditNickname_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditAltNickname_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditUserName_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditRealName_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_comboBoxGender_currentIndexChanged(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_spinBoxAge_valueChanged(int)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_checkBoxAllowBrowse_clicked()
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditEmail_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditMSN_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditYahoo_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditICQ_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditAOL_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditJabber_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditMySpace_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditCountry_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditStateProvince_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditCity_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditLatitude_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditLonitude_textEdited(QString)
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_lineEditInterest_textEdited(QString)
{
	if(ui->lineEditInterest->text() == "")
	{
		ui->pushButtonInterestsAdd->setEnabled(false);
	}
	else
	{
		ui->pushButtonInterestsAdd->setEnabled(true);
	}
}

void DialogProfile::on_pushButtonInterestsAdd_clicked()
{
	if(ui->lineEditInterest->text() != "")
	{
		ui->listWidgetInterests->addItem(ui->lineEditInterest->text());
		ui->pushButtonApply->setEnabled(true);
	}
}

void DialogProfile::on_pushButtonInterestsRemove_clicked()
{
	if(ui->listWidgetInterests->currentRow() != -1)
	{
		ui->listWidgetRemoved->addItem(ui->listWidgetInterests->currentItem()->text());
		ui->listWidgetInterests->takeItem(ui->listWidgetInterests->currentRow());
		ui->pushButtonApply->setEnabled(true);
	}
}

void DialogProfile::on_listWidgetRemoved_itemClicked(QListWidgetItem* item)
{
	ui->lineEditInterest->setText(item->text());
	ui->pushButtonInterestsAdd->setEnabled(true);
}

void DialogProfile::on_plainTextEditBio_textChanged()
{
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_pushButtonAvatarAdd_clicked()
{
	m_sTempAvatarFileName = QFileDialog::getOpenFileName(this, tr("Open Avatar Image"),
	                        qApp->applicationDirPath(),
	                        tr("Images") + " (*.bmp *.png *.xbm *.xpm *.jpg *.jpeg *.gif *.pbm *.pgm *.ppm)");
	if(m_sTempAvatarFileName != "")
	{
		ui->labelAvatarPreview->setPixmap(m_sTempAvatarFileName);
		ui->pushButtonApply->setEnabled(true);
	}
}

void DialogProfile::on_pushButtonRemove_clicked()
{
	m_sTempAvatarFileName = ":/Chat/Graphics/Chat/DefaultAvatar.png";
	ui->labelAvatarPreview->setPixmap(QPixmap(m_sTempAvatarFileName));
	ui->pushButtonApply->setEnabled(true);
}

void DialogProfile::on_pushButtonNewWebsite_clicked()
{
	if(ui->lineEditAddress->text() != "" && ui->lineEditWebsite->text() != "")
	{
		QTreeWidgetItem* m_qTreeWidgetItem = new QTreeWidgetItem();
		m_qTreeWidgetItem->setText(0, ui->lineEditWebsite->text());
		m_qTreeWidgetItem->setText(1, ui->lineEditAddress->text());
		ui->treeWidgetFavorites->addTopLevelItem(m_qTreeWidgetItem);
		ui->pushButtonApply->setEnabled(true);
	}
}

void DialogProfile::skinChangeEvent()
{
	ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
	ui->listWidgetFavoriteFiles->setStyleSheet(skinSettings.listViews);
	ui->listWidgetInterests->setStyleSheet(skinSettings.listViews);
	ui->listWidgetRemoved->setStyleSheet(skinSettings.listViews);
	ui->treeWidgetFavorites->setStyleSheet(skinSettings.listViews);
	ui->treeWidgetProfileNavigation->setStyleSheet(skinSettings.listViews);
}

void DialogProfile::on_pushButtonRemoveWebsite_clicked()
{
	if(ui->treeWidgetFavorites->currentIndex().isValid())
	{
		ui->treeWidgetFavorites->takeTopLevelItem(ui->treeWidgetFavorites->indexOfTopLevelItem
		        (ui->treeWidgetFavorites->currentItem()));
		ui->pushButtonApply->setEnabled(true);
	}
}

void DialogProfile::on_pushButtonGenerateNewGUID_clicked()
{
	tempGUID = QUuid::createUuid();
	ui->labelGUID->setText(tempGUID.toString().remove(QRegExp("[{}]")));
	ui->pushButtonApply->setEnabled(true);
}

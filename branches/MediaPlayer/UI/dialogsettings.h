//
// dialogsettings.h
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
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

#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QtGui>

namespace SettingsPage
{
	enum settingsPage
	{
		System, Skins, Library, FileTypes, MediaPlayer, Search, Integration, Plugins, PrivateMessages, Chat, Security,
		Parental, Connection, Transfers, Downloads, Uploads, Gnutella2, Ares, EDonkey, BitTorrent, Protocols
	};
}

namespace Ui
{
	class DialogSettings;
}

class DialogSettings : public QDialog
{
	Q_OBJECT

public:
	explicit DialogSettings(QWidget* parent = 0, SettingsPage::settingsPage page = SettingsPage::System);
	~DialogSettings();

public slots:
	void enableApply();

protected:
	void changeEvent(QEvent* e);

private slots:
	void switchSettingsPage(SettingsPage::settingsPage page);
	void on_listWidgetGeneralTask_clicked(QModelIndex index);
	void on_listWidgetCommunityTask_clicked(QModelIndex index);
	void on_listWidgetSecurityTask_clicked(QModelIndex index);
	void on_listWidgetNetworkTask_clicked(QModelIndex index);
	void on_listWidgetProtocolsTask_clicked(QModelIndex index);
	void on_pushButtonOk_clicked();
	void on_pushButtonCancel_clicked();
	void on_pushButtonApply_clicked();
	 
	void on_pushButtonProfileEdit_clicked();
	void on_pushButtonShowParentalFilter_clicked();
	void on_labelConfigureG2_linkActivated(QString link);
	void on_labelConfigureAres_linkActivated(QString link);
	void on_labelConfigureEDonkey_linkActivated(QString link);
	void on_labelConfigureBitTorrent_linkActivated(QString link);
	void on_listWidgetSkins_itemClicked(QListWidgetItem* item);
	void on_pushButtonSkinPreview_clicked();
	void on_pushButtonFileTypesSafeOpenAdd_clicked();
	void on_pushButtonFileTypesNeverShareAdd_clicked();
	void on_pushButtonAddManageDownloadTypes_clicked();
	void on_pushButtonUserAgentAdd_clicked();
	void on_pushButtonAddParentalFilter_clicked();
	void on_pushButtonFileTypesSafeOpenRemove_clicked();
	void on_pushButtonFileTypesNeverShareRemove_clicked();
	void on_pushButtonRemoveManageDownloadTypes_clicked();
	void on_pushButtonUserAgentRemove_clicked();
	void on_pushButtonRemoveParentalFilter_clicked();
	void on_toolButtonSaveBrowse_clicked();
	void on_toolButtonTempBrowse_clicked();
	void on_toolButtonTorrentBrowse_clicked();

private:
	Ui::DialogSettings* ui;
	QString skinFile;
	QString tempSkinName;
	QString tempSkinAuthor;
	QString tempSkinVersion;
	QString tempSkinDescription;
	bool newSkinSelected;

signals:
	void closed();
	void skinChanged();
};

#endif // DIALOGSETTINGS_H

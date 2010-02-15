//
// dialogprofile.h
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

#ifndef DIALOGPROFILE_H
#define DIALOGPROFILE_H

#include <QtGui/QDialog>
#include <QListWidgetItem>
#include "quazaasettings.h"

namespace Ui {
	class DialogProfile;
}

class DialogProfile : public QDialog {
	Q_OBJECT
	Q_DISABLE_COPY(DialogProfile)
public:
	explicit DialogProfile(QWidget *parent = 0);
	virtual ~DialogProfile();
	QString m_sTempAvatarFileName;

protected:
	virtual void changeEvent(QEvent *e);

private:
	Ui::DialogProfile *m_ui;

signals:
	void closed();

private slots:
	void on_pushButtonGenerateNewGUID_clicked();
	void on_pushButtonRemoveWebsite_clicked();
	void on_pushButtonNewWebsite_clicked();
	void on_pushButtonRemove_clicked();
	void on_pushButtonAvatarAdd_clicked();
	void on_plainTextEditBio_textChanged();
	void on_listWidgetRemoved_itemClicked(QListWidgetItem* item);
	void on_pushButtonInterestsRemove_clicked();
	void on_pushButtonInterestsAdd_clicked();
	void on_lineEditInterest_textEdited(QString );
	void on_lineEditLonitude_textEdited(QString );
	void on_lineEditLatitude_textEdited(QString );
	void on_lineEditCity_textEdited(QString );
	void on_lineEditStateProvince_textEdited(QString );
	void on_lineEditCountry_textEdited(QString );
	void on_lineEditMySpace_textEdited(QString );
	void on_lineEditJabber_textEdited(QString );
	void on_lineEditAOL_textEdited(QString );
	void on_lineEditICQ_textEdited(QString );
	void on_lineEditYahoo_textEdited(QString );
	void on_lineEditMSN_textEdited(QString );
	void on_lineEditEmail_textEdited(QString );
	void on_checkBoxAllowBrowse_clicked();
	void on_spinBoxAge_valueChanged(int );
	void on_comboBoxGender_currentIndexChanged(QString );
	void on_lineEditRealName_textEdited(QString );
	void on_lineEditUserName_textEdited(QString );
	void on_lineEditAltNickname_textEdited(QString );
	void on_lineEditNickname_textEdited(QString );
	void on_pushButtonApply_clicked();
	void on_lineEditGnutellaSceenName_textEdited(QString );
	void on_pushButtonCancel_clicked();
	void on_pushButtonOK_clicked();
	void on_treeWidgetProfileNavigation_itemSelectionChanged();
};

#endif // DIALOGPROFILE_H

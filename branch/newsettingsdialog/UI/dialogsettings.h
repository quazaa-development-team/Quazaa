#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QtGui>

namespace SettingsPage {
	enum settingsPage {
		System, Skins, Library, FileTypes, MediaPlayer, Search, Integration, Plugins, PrivateMessages, Chat, Security,
		Parental, Connection, Transfers, Downloads, Uploads, Gnutella2, Ares, EDonkey, BitTorrent, Protocols
	};
}

namespace Ui {
	class DialogSettings;
}

class DialogSettings : public QDialog
{
	Q_OBJECT

public:
	explicit DialogSettings(QWidget *parent = 0);
	~DialogSettings();

public slots:
	void switchSettingsPage(int page);

protected:
	void changeEvent(QEvent *e);

private slots:
	void on_listWidgetGeneralTask_clicked(QModelIndex index);

	void on_listWidgetCommunityTask_clicked(QModelIndex index);

	void on_listWidgetSecurityTask_clicked(QModelIndex index);

	void on_listWidgetNetworkTask_clicked(QModelIndex index);

	void on_listWidgetProtocolsTask_clicked(QModelIndex index);

private:
	Ui::DialogSettings *ui;
};

#endif // DIALOGSETTINGS_H

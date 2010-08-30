//
// widgethome.h
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

#ifndef WIDGETHOME_H
#define WIDGETHOME_H

#include <QWidget>

namespace Ui {
    class WidgetHome;
}

class WidgetHome : public QWidget {
    Q_OBJECT
public:
    WidgetHome(QWidget *parent = 0);
    ~WidgetHome();
	void saveWidget();
	void mouseDoubleClickEvent(QMouseEvent *);

signals:
	void triggerLibrary();
	void triggerTransfers();
	void triggerSecurity();
	void requestSearch(QString *searchString);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetHome *ui;

private slots:
 void on_splitterHome_customContextMenuRequested(QPoint pos);
 void on_toolButtonWelcomeSearch_clicked();
 void on_labelWelcomeUserGuideLink_linkActivated(QString link);
 void on_labelWelcomeWizardLink_linkActivated(QString link);
 void on_labelWelcomeSkinLink_linkActivated(QString link);
 void on_labelWelcomeOpenTorrentLink_linkActivated(QString link);
 void on_labelWelcomeURLDownloadLink_linkActivated(QString link);
 void on_labelTorrentsTaskSeedTorrentLink_linkActivated(QString link);
 void on_labelLibraryTaskHashFaster_linkActivated(QString link);
 void skinChangeEvent();
};

#endif // WIDGETHOME_H

//
// widgetsearch.h
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

#ifndef WIDGETSEARCH_H
#define WIDGETSEARCH_H

#include <QWidget>
#include "widgetsearchresults.h"

namespace Ui {
    class WidgetSearch;
}

class WidgetSearch : public QWidget {
    Q_OBJECT
public:
    WidgetSearch(QWidget *parent = 0);
    ~WidgetSearch();
	WidgetSearchResults *panelSearchResults;
	void saveWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSearch *ui;

public slots:
	void startNewSearch(QString *searchString);

private slots:
	void on_splitterSearch_customContextMenuRequested(QPoint pos);
	void on_toolButtonNewSearch_clicked();
	void on_toolButtonSearchClear_clicked();
	void on_toolButtonSearch_clicked();
	void skinChangeEvent();
	void onSearchTabChanged(WidgetSearchTemplate *searchPage);
	void updateStats(WidgetSearchTemplate *searchWidget);
};

#endif // WIDGETSEARCH_H

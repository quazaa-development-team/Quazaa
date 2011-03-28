//
// widgetsearchresults.h
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

#ifndef WIDGETSEARCHRESULTS_H
#define WIDGETSEARCHRESULTS_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include "widgetsearchtemplate.h"

namespace Ui
{
	class WidgetSearchResults;
}

class WidgetSearchResults : public QMainWindow
{
	Q_OBJECT
public:
	WidgetSearchResults(QWidget* parent = 0);
	~WidgetSearchResults();
	QLabel* labelFilter;
	QLineEdit* lineEditFilter;
	void saveWidget();

signals:
	void searchTabChanged(WidgetSearchTemplate* searchPage);
	void statsUpdated(WidgetSearchTemplate* searchPage);

protected:
	void changeEvent(QEvent* e);

private:
	Ui::WidgetSearchResults* ui;

public slots:
	void skinChangeEvent();
	void startSearch(QString searchString);
	void startNewSearch(QString* searchString);
	void addSearchTab();
	void stopSearch();
	bool clearSearch();
	void onStatsUpdated(WidgetSearchTemplate* searchWidget);

private slots:
	void on_tabWidgetSearch_currentChanged(int index);
	void on_splitterSearchDetails_customContextMenuRequested(QPoint pos);
	void on_actionFilterMore_triggered();
	void on_tabWidgetSearch_tabCloseRequested(int index);
};

#endif // WIDGETSEARCHRESULTS_H

//
// widgetlibrary.h
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

#ifndef WIDGETLIBRARY_H
#define WIDGETLIBRARY_H

#include <QWidget>
#include "widgetlibraryview.h"

namespace Ui
{
	class WidgetLibrary;
}

class WidgetLibrary : public QWidget
{
	Q_OBJECT
public:
	WidgetLibrary(QWidget* parent = 0);
	~WidgetLibrary();
	WidgetLibraryView* panelLibraryView;
	void saveWidget();

protected:
	void changeEvent(QEvent* e);

private:
	Ui::WidgetLibrary* ui;

public slots:
	void initializeLibrary();

private slots:
	void on_splitterLibrary_customContextMenuRequested(QPoint pos);
	void on_toolButtonLibraryEditShares_clicked();
	void skinChangeEvent();
};

#endif // WIDGETLIBRARY_H

//
// widgetsearchtemplate.h
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

#ifndef WIDGETSEARCHTEMPLATE_H
#define WIDGETSEARCHTEMPLATE_H

#include <QtGui/QDialog>
#include "searchtreemodel.h"

namespace Ui
{
	class WidgetSearchTemplate;
}

class CManagedSearch;
class CQuery;
//class CQueryHit;
#include "NetworkCore/queryhit.h"
#include "types.h"

namespace SearchState
{
	enum SearchState { Stopped, Searching, Paused };
};

class WidgetSearchTemplate : public QWidget
{
	Q_OBJECT

public:
	WidgetSearchTemplate(QString searchString = "", QWidget* parent = 0);
	~WidgetSearchTemplate();
	SearchTreeModel* searchModel;
	CManagedSearch* m_pSearch;

	void StartSearch(CQuery* pQuery);
	void PauseSearch();
	void StopSearch();
	void ClearSearch();

	int nHubs;
	int nLeaves;
	int nHits;
	int nFiles;

	QString sSearchString;
	SearchState::SearchState searchState;

	//void GetStats(quint32& nHubs, quint32& nLeaves, quint32& nHits);

signals:
	void statsUpdated(WidgetSearchTemplate* thisSearch);

protected:
	void changeEvent(QEvent* e);

private:
	Ui::WidgetSearchTemplate* m_ui;

protected slots:
	void OnStatsUpdated();
	void skinChangeEvent();
};

#endif // WIDGETSEARCHTEMPLATE_H

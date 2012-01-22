/*
** widgetsearchtemplate.h
**
** Copyright © Quazaa Development Team, 2009-2012.
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

#ifndef WIDGETSEARCHTEMPLATE_H
#define WIDGETSEARCHTEMPLATE_H

#include <QtGui/QDialog>
#include <QSortFilterProxyModel>
#include <QMenu>
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
        enum SearchState { Default, Stopped, Searching, Paused };
};

class WidgetSearchTemplate : public QWidget
{
	Q_OBJECT

public:
	WidgetSearchTemplate(QString searchString = "", QWidget* parent = 0);
	~WidgetSearchTemplate();
	SearchTreeModel* searchModel;
	QSortFilterProxyModel* sortModel;
	CManagedSearch* m_pSearch;

	void StartSearch(CQuery* pQuery);
	void PauseSearch();
	void StopSearch();
	void ClearSearch();
	QModelIndex CurrentItem();

	int nHubs;
	int nLeaves;
	int nHits;
	int nFiles;

	QString sSearchString;
	SearchState::SearchState searchState;

	//void GetStats(quint32& nHubs, quint32& nLeaves, quint32& nHits);

signals:
	void statsUpdated(WidgetSearchTemplate* thisSearch);
	void stateChanged();

protected:
	void changeEvent(QEvent* e);

private:
	Ui::WidgetSearchTemplate* ui;

	QMenu *searchMenu;

protected slots:
	void OnStatsUpdated();
	void OnStateChanged();
	void Sort();

public slots:
	void saveHeaderState();
	void loadHeaderState();
private slots:
	void on_treeViewSearchResults_doubleClicked(const QModelIndex &index);
	void on_treeViewSearchResults_customContextMenuRequested(const QPoint &pos);
	void on_actionDownload_triggered();
	void on_actionViewReviews_triggered();
	void on_actionVirusTotalCheck_triggered();
};

#endif // WIDGETSEARCHTEMPLATE_H

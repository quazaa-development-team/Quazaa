//
// widgetsearchtemplate.cpp
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

#include "widgetsearchtemplate.h"
#include "ui_widgetsearchtemplate.h"

#include "NetworkCore/SearchManager.h"
#include "NetworkCore/ManagedSearch.h"
#include "NetworkCore/Query.h"
#include "NetworkCore/QueryHit.h"

WidgetSearchTemplate::WidgetSearchTemplate(QWidget *parent) :
	QWidget(parent),
    m_ui(new Ui::WidgetSearchTemplate)
{
	m_ui->setupUi(this);
	m_pSearch = 0;
	searchModel = new SearchTreeModel();
	m_ui->treeViewSearchResults->setModel(searchModel);
}

WidgetSearchTemplate::~WidgetSearchTemplate()
{
	if ( m_pSearch != 0 )
		StopSearch();
	delete m_ui;
}

void WidgetSearchTemplate::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void WidgetSearchTemplate::StartSearch(CQuery *pQuery)
{
    if( m_pSearch && m_pSearch->m_pQuery != pQuery )
    {
        delete m_pSearch;
        m_pSearch = 0;
    }

    if( !m_pSearch )
    {
        m_pSearch = new CManagedSearch(pQuery);
		connect(m_pSearch, SIGNAL(OnHit(QueryHitSharedPtr)), searchModel, SLOT(addQueryHit(QueryHitSharedPtr)));
		connect(m_pSearch, SIGNAL(StatsUpdated()), this, SLOT(OnStatsUpdated()));
    }

	m_pSearch->Start();
}

void WidgetSearchTemplate::StopSearch()
{
    Q_ASSERT(m_pSearch != 0);

    m_pSearch->Stop();
    delete m_pSearch;
    m_pSearch = 0;
}

void WidgetSearchTemplate::PauseSearch()
{
    Q_ASSERT(m_pSearch != 0);

    m_pSearch->Pause();
}

void WidgetSearchTemplate::OnStatsUpdated()
{

}

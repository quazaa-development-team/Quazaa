/*
** $Id$
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

#include "widgetdownloads.h"
#include "ui_widgetdownloads.h"

#include "quazaasettings.h"
#include "quazaaglobals.h"
#include "downloadstreemodel.h"
#include "skinsettings.h"

#include <QFontMetrics>

#ifdef _DEBUG
#include "debug_new.h"
#endif

WidgetDownloads::WidgetDownloads(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetDownloads)
{
	ui->setupUi(this);
	downloadMenu = new QMenu(this);
	downloadMenu->addAction(ui->actionPreview);
	downloadMenu->addAction(ui->actionAddToPlaylist);
	downloadMenu->addAction(ui->actionShowMonitor);
	downloadMenu->addSeparator();
	downloadMenu->addAction(ui->actionResume);
	downloadMenu->addAction(ui->actionPause);
	downloadMenu->addAction(ui->actionCancel);
	downloadMenu->addSeparator();
	downloadMenu->addAction(ui->actionFindMoreSources);
	downloadMenu->addSeparator();
	downloadMenu->addAction(ui->actionViewReviews);
	downloadMenu->addAction(ui->actionVirusTotalCheck);
	downloadMenu->addSeparator();
	downloadMenu->addAction(ui->actionHelp);

	m_pModel = new CDownloadsTreeModel();
	ui->treeViewDownloads->setModel(m_pModel);
	ui->treeViewDownloads->setItemDelegate(new CDownloadsItemDelegate(this));
	setSkin();

	// Set up header sizes
	if(!ui->treeViewDownloads->header()->restoreState(quazaaSettings.WinMain.DownloadsHeader))
	{
		QFontMetrics fm = ui->treeViewDownloads->fontMetrics();
		QHeaderView* header = ui->treeViewDownloads->header();
		header->resizeSection(CDownloadsTreeModel::NAME, fm.width("a-typical-name-of-mp3-or-movie-or-xxx-or-other-porn.avi"));
		header->resizeSection(CDownloadsTreeModel::SIZE, fm.width(" 123.45 MB "));
		header->resizeSection(CDownloadsTreeModel::COMPLETED, fm.width(" 1234.45 MB "));
		header->resizeSection(CDownloadsTreeModel::BANDWIDTH, fm.width(" 1234.45 MB/s "));
		header->resizeSection(CDownloadsTreeModel::CLIENT, fm.width(QuazaaGlobals::USER_AGENT_STRING()));
		header->resizeSection(CDownloadsTreeModel::PROGRESS, 200);
		header->resizeSection(CDownloadsTreeModel::PRIORITY, fm.width("    Priority    "));
		header->resizeSection(CDownloadsTreeModel::STATUS, fm.width("Tracker Down"));
	}

	restoreState(quazaaSettings.WinMain.DownloadsToolbar);
}

WidgetDownloads::~WidgetDownloads()
{
	delete m_pModel;
	delete ui;
}

void WidgetDownloads::changeEvent(QEvent* e)
{
	QMainWindow::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void WidgetDownloads::saveWidget()
{
	quazaaSettings.WinMain.DownloadsToolbar = saveState();
	quazaaSettings.WinMain.DownloadsHeader = ui->treeViewDownloads->header()->saveState();
}


void WidgetDownloads::on_treeViewDownloads_customContextMenuRequested(const QPoint &pos)
{
	QModelIndex currIndex = ui->treeViewDownloads->indexAt(pos);
	if( currIndex.isValid() )
	{
		downloadMenu->exec(QCursor::pos());
	}
}

void WidgetDownloads::setSkin()
{
	ui->treeViewDownloads->setStyleSheet(skinSettings.listViews);
}

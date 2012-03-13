/*
** $Id: widgethostcache.cpp 931 2012-02-18 00:32:03Z smokexyz $
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

#include "widgethostcache.h"
#include "ui_widgethostcache.h"

#include "quazaasettings.h"
#include "skinsettings.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

WidgetHostCache::WidgetHostCache(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetHostCache)
{
	ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.HostCacheToolbar);
	ui->splitterHostCache->restoreState(quazaaSettings.WinMain.HostCacheSplitter);
	setSkin();
}

WidgetHostCache::~WidgetHostCache()
{
	delete ui;
}

void WidgetHostCache::changeEvent(QEvent* e)
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

void WidgetHostCache::saveWidget()
{
	quazaaSettings.WinMain.HostCacheToolbar = saveState();
	quazaaSettings.WinMain.HostCacheSplitter = ui->splitterHostCache->saveState();
}

void WidgetHostCache::on_splitterHostCache_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	if(ui->splitterHostCache->handle(1)->underMouse())
	{
		if(ui->splitterHostCache->sizes()[0] > 0)
		{
			quazaaSettings.WinMain.HostCacheSplitterRestoreLeft = ui->splitterHostCache->sizes()[0];
			quazaaSettings.WinMain.HostCacheSplitterRestoreRight = ui->splitterHostCache->sizes()[1];
			QList<int> newSizes;
			newSizes.append(0);
			newSizes.append(ui->splitterHostCache->sizes()[0] + ui->splitterHostCache->sizes()[1]);
			ui->splitterHostCache->setSizes(newSizes);
		}
		else
		{
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.HostCacheSplitterRestoreLeft);
			sizesList.append(quazaaSettings.WinMain.HostCacheSplitterRestoreRight);
			ui->splitterHostCache->setSizes(sizesList);
		}
	}
}

void WidgetHostCache::setSkin()
{

}

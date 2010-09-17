//
// widgethostcache.cpp
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

#include "widgethostcache.h"
#include "ui_widgethostcache.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetHostCache::WidgetHostCache(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetHostCache)
{
	ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.HostCacheToolbar);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
	ui->splitterHostCache->restoreState(quazaaSettings.WinMain.HostCacheSplitter);
}

WidgetHostCache::~WidgetHostCache()
{
	delete ui;
}

void WidgetHostCache::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void WidgetHostCache::skinChangeEvent()
{
	ui->toolBar->setStyleSheet(skinSettings.toolbars);
}

void WidgetHostCache::saveWidget()
{
	quazaaSettings.WinMain.HostCacheToolbar = saveState();
	quazaaSettings.WinMain.HostCacheSplitter = ui->splitterHostCache->saveState();
}

void WidgetHostCache::on_splitterHostCache_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	if (ui->splitterHostCache->handle(1)->underMouse())
	{
		if (ui->splitterHostCache->sizes()[0] > 0)
		{
			quazaaSettings.WinMain.HostCacheSplitterRestoreLeft = ui->splitterHostCache->sizes()[0];
			quazaaSettings.WinMain.HostCacheSplitterRestoreRight = ui->splitterHostCache->sizes()[1];
			QList<int> newSizes;
			newSizes.append(0);
			newSizes.append(ui->splitterHostCache->sizes()[0] + ui->splitterHostCache->sizes()[1]);
			ui->splitterHostCache->setSizes(newSizes);
		} else {
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.HostCacheSplitterRestoreLeft);
			sizesList.append(quazaaSettings.WinMain.HostCacheSplitterRestoreRight);
			ui->splitterHostCache->setSizes(sizesList);
		}
	}
}

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

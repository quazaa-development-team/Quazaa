#include "widgetpacketdump.h"
#include "ui_widgetpacketdump.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetPacketDump::WidgetPacketDump(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetPacketDump)
{
    ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.PacketDumpToolbar);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

WidgetPacketDump::~WidgetPacketDump()
{
    delete ui;
}

void WidgetPacketDump::changeEvent(QEvent *e)
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

void WidgetPacketDump::skinChangeEvent()
{
	ui->toolBar->setStyleSheet(skinSettings.toolbars);
}

void WidgetPacketDump::saveWidget()
{
	quazaaSettings.WinMain.PacketDumpToolbar = saveState();
}


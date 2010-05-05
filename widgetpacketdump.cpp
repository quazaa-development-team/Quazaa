#include "widgetpacketdump.h"
#include "ui_widgetpacketdump.h"
#include "QSkinDialog/qskinsettings.h"

WidgetPacketDump::WidgetPacketDump(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetPacketDump)
{
    ui->setupUi(this);
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

void WidgetPacketDump::appendLog(QString *message, LogSeverity::Severity severity)
{
	switch (severity)
	{
	case LogSeverity::Information:

		break;
	case LogSeverity::Security:

		break;
	case LogSeverity::Notice:

		break;
	case LogSeverity::Debug:

		break;
	case LogSeverity::Warning:

		break;
	case LogSeverity::Error:

		break;
	case LogSeverity::Critical:

		break;
	}
}

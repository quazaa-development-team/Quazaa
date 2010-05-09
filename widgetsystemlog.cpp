#include "widgetsystemlog.h"
#include "ui_widgetsystemlog.h"
#include "QSkinDialog/qskinsettings.h"

WidgetSystemLog::WidgetSystemLog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetSystemLog)
{
    ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	connect(&systemLog, SIGNAL(logPosted(QString,LogSeverity::Severity)), this, SLOT(appendLog(QString,LogSeverity::Severity)));
	skinChangeEvent();
}

WidgetSystemLog::~WidgetSystemLog()
{
	delete ui;
}

void WidgetSystemLog::changeEvent(QEvent *e)
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

void WidgetSystemLog::skinChangeEvent()
{
	ui->toolBar->setStyleSheet(skinSettings.toolbars);
}

void WidgetSystemLog::appendLog(QString message, LogSeverity::Severity severity)
{
	switch (severity)
	{
	case LogSeverity::Information:
		ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 %2\">%3</span>").arg(skinSettings.logWeightInformation).arg(skinSettings.logColorInformation).arg(message));
		break;
	case LogSeverity::Security:
		ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 %2\">%3</span>").arg(skinSettings.logWeightSecurity).arg(skinSettings.logColorSecurity).arg(message));
		break;
	case LogSeverity::Notice:
		ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 %2\">%3</span>").arg(skinSettings.logWeightNotice).arg(skinSettings.logColorNotice).arg(message));
		break;
	case LogSeverity::Debug:
		ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 %2\">%3</span>").arg(skinSettings.logWeightDebug).arg(skinSettings.logColorDebug).arg(message));
		break;
	case LogSeverity::Warning:
		ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 %2\">%3</span>").arg(skinSettings.logWeightWarning).arg(skinSettings.logColorWarning).arg(message));
		break;
	case LogSeverity::Error:
		ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 %2\">%3</span>").arg(skinSettings.logWeightError).arg(skinSettings.logColorError).arg(message));
		break;
	case LogSeverity::Critical:
		ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 %2\">%3</span>").arg(skinSettings.logWeightCritical).arg(skinSettings.logColorCritical).arg(message));
		break;
	}
}

void WidgetSystemLog::saveState()
{

}

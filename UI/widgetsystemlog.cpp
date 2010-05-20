#include "widgetsystemlog.h"
#include "ui_widgetsystemlog.h"

#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

WidgetSystemLog::WidgetSystemLog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WidgetSystemLog)
{
    ui->setupUi(this);
	restoreState(quazaaSettings.WinMain.SystemLogToolbar);
	ui->actionToggleTimestamp->setChecked(quazaaSettings.Logging.LogShowTimestamp);
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
	if (ui->actionToggleTimestamp->isChecked())
	{
		timeStamp = QTime::currentTime();
		switch (severity)
		{
		case LogSeverity::Information:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightInformation).arg(skinSettings.logColorInformation.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
			break;
		case LogSeverity::Security:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightSecurity).arg(skinSettings.logColorSecurity.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
			break;
		case LogSeverity::Notice:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightNotice).arg(skinSettings.logColorNotice.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
			break;
		case LogSeverity::Debug:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightDebug).arg(skinSettings.logColorDebug.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
			break;
		case LogSeverity::Warning:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightWarning).arg(skinSettings.logColorWarning.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
			break;
		case LogSeverity::Error:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightError).arg(skinSettings.logColorError.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
			break;
		case LogSeverity::Critical:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg(skinSettings.logWeightCritical).arg(skinSettings.logColorCritical.name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
			break;
		}
	} else {
		switch (severity)
		{
		case LogSeverity::Information:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightInformation).arg(skinSettings.logColorInformation.name()).arg(message));
			break;
		case LogSeverity::Security:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightSecurity).arg(skinSettings.logColorSecurity.name()).arg(message));
			break;
		case LogSeverity::Notice:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightNotice).arg(skinSettings.logColorNotice.name()).arg(message));
			break;
		case LogSeverity::Debug:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightDebug).arg(skinSettings.logColorDebug.name()).arg(message));
			break;
		case LogSeverity::Warning:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightWarning).arg(skinSettings.logColorWarning.name()).arg(message));
			break;
		case LogSeverity::Error:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightError).arg(skinSettings.logColorError.name()).arg(message));
			break;
		case LogSeverity::Critical:
			ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg(skinSettings.logWeightCritical).arg(skinSettings.logColorCritical.name()).arg(message));
			break;
		}
	}
}

void WidgetSystemLog::saveWidget()
{
	quazaaSettings.WinMain.SystemLogToolbar = saveState();
	quazaaSettings.Logging.LogShowTimestamp = ui->actionToggleTimestamp->isChecked();
}

void WidgetSystemLog::on_actionClearBuffer_triggered()
{
	ui->textEditSystemLog->clear();
}

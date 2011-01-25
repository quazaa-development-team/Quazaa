//
// widgetsystemlog.cpp
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

#include "widgetsystemlog.h"
#include "ui_widgetsystemlog.h"

#include "quazaasettings.h"

#include <QMenu>
 

WidgetSystemLog::WidgetSystemLog(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetSystemLog)
{
	ui->setupUi(this);
	quazaaSettings.loadLogSettings();
	ui->actionPauseLogDisplay->setChecked(quazaaSettings.Logging.IsPaused);
	logMenu = new QMenu(ui->textEditSystemLog);
	logMenu->addAction(ui->actionShowInformation);
	ui->actionShowInformation->setChecked(quazaaSettings.Logging.ShowInformation);
	logMenu->addAction(ui->actionShowSecurity);
	ui->actionShowSecurity->setChecked(quazaaSettings.Logging.ShowSecurity);
	logMenu->addAction(ui->actionShowNotice);
	ui->actionShowNotice->setChecked(quazaaSettings.Logging.ShowNotice);
	logMenu->addAction(ui->actionShowDebug);
	ui->actionShowDebug->setChecked(quazaaSettings.Logging.ShowDebug);
	logMenu->addAction(ui->actionShowWarnings);
	ui->actionShowWarnings->setChecked(quazaaSettings.Logging.ShowWarnings);
	logMenu->addAction(ui->actionShowError);
	ui->actionShowError->setChecked(quazaaSettings.Logging.ShowError);
	logMenu->addAction(ui->actionShowCritical);
	ui->actionShowCritical->setChecked(quazaaSettings.Logging.ShowCritical);
	restoreState(quazaaSettings.WinMain.SystemLogToolbar);
	ui->actionToggleTimestamp->setChecked(quazaaSettings.Logging.LogShowTimestamp);
	connect(&systemLog, SIGNAL(logPosted(QString, LogSeverity::Severity)), this, SLOT(appendLog(QString, LogSeverity::Severity)));
}

WidgetSystemLog::~WidgetSystemLog()
{
	delete ui;
}

void WidgetSystemLog::changeEvent(QEvent* e)
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

void WidgetSystemLog::appendLog(QString message, LogSeverity::Severity severity)
{
	if(!ui->actionPauseLogDisplay->isChecked())
	{
		if(ui->actionToggleTimestamp->isChecked())
		{
			timeStamp = QTime::currentTime();
			switch(severity)
			{
				case LogSeverity::Information:
					if(ui->actionShowInformation->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg("font-weight:normal;").arg(QColor(qRgb(0, 0, 0)).name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
					}
					break;
				case LogSeverity::Security:
					if(ui->actionShowSecurity->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg("font-weight:600;").arg(QColor(qRgb(170, 170, 0)).name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
					}
					break;
				case LogSeverity::Notice:
					if(ui->actionShowNotice->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg("font-weight:600;").arg(QColor(qRgb(0, 170, 0)).name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
					}
					break;
				case LogSeverity::Debug:
					if(ui->actionShowDebug->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg("font-weight:normal;").arg(QColor(qRgb(117, 117, 117)).name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
					}
					break;
				case LogSeverity::Warning:
					if(ui->actionShowWarnings->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg("font-weight:normal;").arg(QColor(qRgb(255, 0, 0)).name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
					}
					break;
				case LogSeverity::Error:
					if(ui->actionShowError->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg("font-weight:600;").arg(QColor(qRgb(170, 0, 0)).name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
					}
					break;
				case LogSeverity::Critical:
					if(ui->actionShowCritical->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3: %4</span>").arg("font-weight:600;").arg(QColor(qRgb(255, 0, 0)).name()).arg(timeStamp.toString("hh:mm:ss.zzz")).arg(message));
					}
					break;
			}
		}
		else
		{
			switch(severity)
			{
				case LogSeverity::Information:
					if(ui->actionShowInformation->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg("font-weight:normal;").arg(QColor(qRgb(0, 0, 0)).name()).arg(message));
					}
					break;
				case LogSeverity::Security:
					if(ui->actionShowSecurity->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg("font-weight:600;").arg(QColor(qRgb(170, 170, 0)).name()).arg(message));
					}
					break;
				case LogSeverity::Notice:
					if(ui->actionShowNotice->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg("font-weight:600;").arg(QColor(qRgb(0, 170, 0)).name()).arg(message));
					}
					break;
				case LogSeverity::Debug:
					if(ui->actionShowDebug->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg("font-weight:normal;").arg(QColor(qRgb(117, 117, 117)).name()).arg(message));
					}
					break;
				case LogSeverity::Warning:
					if(ui->actionShowWarnings->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg("font-weight:normal;").arg(QColor(qRgb(255, 0, 0)).name()).arg(message));
					}
					break;
				case LogSeverity::Error:
					if(ui->actionShowError->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg("font-weight:600;").arg(QColor(qRgb(170, 0, 0)).name()).arg(message));
					}
					break;
				case LogSeverity::Critical:
					if(ui->actionShowCritical->isChecked())
					{
						ui->textEditSystemLog->append(QString("<span style=\" font-size:8pt; %1 color:%2;\">%3</span>").arg("font-weight:600;").arg(QColor(qRgb(255, 0, 0)).name()).arg(message));
					}
					break;
			}
		}
	}
}

void WidgetSystemLog::saveWidget()
{
	quazaaSettings.WinMain.SystemLogToolbar = saveState();
	quazaaSettings.Logging.LogShowTimestamp = ui->actionToggleTimestamp->isChecked();
	quazaaSettings.Logging.ShowWarnings = ui->actionShowWarnings->isChecked();
	quazaaSettings.Logging.ShowInformation = ui->actionShowInformation->isChecked();
	quazaaSettings.Logging.ShowSecurity = ui->actionShowSecurity->isChecked();
	quazaaSettings.Logging.ShowNotice = ui->actionShowNotice->isChecked();
	quazaaSettings.Logging.ShowDebug = ui->actionShowDebug->isChecked();
	quazaaSettings.Logging.ShowError = ui->actionShowError->isChecked();
	quazaaSettings.Logging.ShowCritical = ui->actionShowCritical->isChecked();
	quazaaSettings.Logging.IsPaused = ui->actionPauseLogDisplay->isChecked();
	quazaaSettings.saveLogSettings();
}

void WidgetSystemLog::on_actionClearBuffer_triggered()
{
	ui->textEditSystemLog->clear();
}

void WidgetSystemLog::on_textEditSystemLog_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);
	logMenu->exec(QCursor::pos());
}

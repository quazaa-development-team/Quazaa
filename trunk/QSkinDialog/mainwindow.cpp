/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: mainwindow.cpp
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : The main window of the example. Shows usage of qskindialog on child
 *				  forms and opens the simple editor.
 ********************************************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qskindialog.h"
#include "qskinconfiguredialog.h"
#include "qskineditordialog.h"
#include "qskinsettings.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::changeEvent(QEvent *e)
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

void MainWindow::on_actionChangeSkin_triggered()
{
	QSkinDialog *skinConfigureDialog = new QSkinDialog(false, true, this);
	QSkinConfigureDialog *configureDialog = new QSkinConfigureDialog(this);
	skinConfigureDialog->addChildWidget(configureDialog);
	connect(configureDialog, SIGNAL(closed()), skinConfigureDialog, SLOT(onClose()));
	skinConfigureDialog->exec();
}


void MainWindow::on_actionShowSkinEditor_triggered()
{
	QSkinEditorDialog *skinEditorDialog = new QSkinEditorDialog(false, true, this);
	skinEditorDialog->show();
}

/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: mainwindow.h
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : Header for the main window of the example. Shows usage of qskindialog
 *				  on child forms and opens the simple editor.
 ********************************************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

signals:
	void closed();
	void skinChanged();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::MainWindow *ui;

private slots:
	void on_actionShowSkinEditor_triggered();
 void on_actionChangeSkin_triggered();
};

#endif // MAINWINDOW_H

/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: qskinconfiguredialog.h
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : Header for an example configuration dialog for QSkinDialog.
 ********************************************************************************************************/
#ifndef QSKINCONFIGUREDIALOG_H
#define QSKINCONFIGUREDIALOG_H

#include <QtGui>
#include <QPoint>

namespace Ui {
	class QSkinConfigureDialog;
}

class QSkinConfigureDialog : public QDialog {
	Q_OBJECT
public:
	QSkinConfigureDialog(QWidget *parent = 0);
	~QSkinConfigureDialog();

signals:
	void closed();
	void skinChanged();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::QSkinConfigureDialog *ui;
	QString skinFile;
	QString tempSkinName;
	QString tempSkinAuthor;
	QString tempSkinVersion;
	QString tempSkinDescription;
	QString tempWindowFrameTopLeftStyleSheet;
	QString tempWindowFrameLeftStyleSheet;
	QString tempWindowFrameBottomLeftStyleSheet;
	QString tempWindowFrameTopStyleSheet;
	QString tempWindowFrameBottomStyleSheet;
	QString tempWindowFrameTopRightStyleSheet;
	QString tempWindowFrameRightStyleSheet;
	QString tempWindowFrameBottomRightStyleSheet;
	QString tempTitlebarFrameStyleSheet;
	QString tempTitlebarButtonsFrameStyleSheet;
	QString tempMinimizeButtonStyleSheet;
	QString tempMaximizeButtonStyleSheet;
	QString tempCloseButtonStyleSheet;
	QString tempWindowTextStyleSheet;
	bool tempWindowIconVisible;
	QSize tempWindowIconSize;

private slots:
	void on_pushButtonOK_clicked();
 void on_pushButtonPreview_clicked();
 void on_listWidgetSkins_itemClicked(QListWidgetItem* item);
};

#endif // QSKINCONFIGUREDIALOG_H

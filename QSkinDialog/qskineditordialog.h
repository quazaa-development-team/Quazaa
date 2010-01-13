/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: qskineditordialog.h
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : Header for a simple editor without the ability to actually edit or create
 *				  a skin. It just creates a skin file (*.qsf) based on the current skin
 *				  which is created at design time.
 ********************************************************************************************************/
#ifndef QSKINEDITORDIALOG_H
#define QSKINEDITORDIALOG_H

#include <QtGui>
#include <QPoint>

namespace Ui {
	class QSkinEditorDialog;
}

class QSkinEditorDialog : public QDialog {
	Q_OBJECT
public:
	QSkinEditorDialog(bool sizable = true, bool closable = true, QWidget *parent = 0);
	~QSkinEditorDialog();
	void addChildWidget(QWidget *parent = 0);

protected:
	void changeEvent(QEvent *e);
	void skinChangeEvent();

private:
	Ui::QSkinEditorDialog *ui;
	QPoint dragPosition;
	int iDragPosition;
	QRect normalGeometry;
	bool maximized;
	bool minimized;
	bool movable;
	bool dialogSizable;
	bool dialogClosable;
	bool sizableTopLeft;
	bool sizableLeft;
	bool sizableBottomLeft;
	bool sizableBottom;
	bool sizableTopRight;
	bool sizableRight;
	bool sizableBottomRight;
	QMenu *systemMenu;
	QAction *systemRestoreAction;
	QAction *systemMinimizeAction;
	QAction *systemMaximizeAction;
	QAction *systemCloseAction;

private slots:
	void on_windowIcon_clicked();
	void on_windowIcon_customContextMenuRequested(QPoint pos);
	void on_windowFrameTop_customContextMenuRequested(QPoint pos);
	void on_windowText_customContextMenuRequested(QPoint pos);
	void onClose();
	void onMaximize();
	void setMaximized(bool maximize);
	void onMinimize();
	void setMinimized(bool minimize);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);
	void loadSkin(QString file = "");
	void saveSkin();
	void setWindowTitle(const QString &);
	void setWindowIcon(const QIcon &icon);
};

#endif // QSKINEDITORDIALOG_H

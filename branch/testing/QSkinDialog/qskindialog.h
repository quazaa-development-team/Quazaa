/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: qskindialog.h
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : Header for the core of QSkinDialog, a library allowing skinning in Qt through
 *				  style sheets. It is written for reuse in multiple programs.
 ********************************************************************************************************/
#ifndef QSKINDIALOG_H
#define QSKINDIALOG_H

#include <QtGui>
#include <QPoint>

namespace Ui {
	class QSkinDialog;
}

class QSkinDialog : public QDialog {
	Q_OBJECT
public:
	QSkinDialog(bool sizable = false, bool closable = false, bool mainDialog = false, QWidget *parent = 0);
	~QSkinDialog();
	void addChildWidget(QWidget *parent = 0);
	QRect dialogNormalGeometry() {return this->normalGeometry;}
	bool isMinimized() { return minimized; }
	bool isMaximized() { return maximized; }

protected:
	void changeEvent(QEvent *e);

signals:
	void mainClose();
	void needToShow();

private:
	Ui::QSkinDialog *ui;
	QPoint dragPosition;
	int iDragPosition;
	QRect normalGeometry;
	bool maximized;
	bool minimized;
	bool movable;
	bool dialogSizable;
	bool dialogClosable;
	bool isMainDialog;
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

public slots:
	void setMaximized(bool maximize);
	void setMinimized(bool minimize);
	void restore();

private slots:
	void on_windowFrameTopSpacer_customContextMenuRequested(QPoint pos);
        void on_titlebarButtonsFrame_customContextMenuRequested(QPoint pos);
	void on_windowIconFrame_customContextMenuRequested(QPoint pos);
	void on_windowIcon_clicked();
	void on_windowIcon_customContextMenuRequested(QPoint pos);
	void on_windowFrameTop_customContextMenuRequested(QPoint pos);
	void on_windowText_customContextMenuRequested(QPoint pos);
	void onClose();
	void onMaximize();
	void onMinimize();
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);
	void loadSkin(QString file = "");
	void setWindowTitle(const QString &);
	void setWindowIcon(const QIcon &icon);
	void skinChangeEvent();
};

#endif // QSKINDIALOG_H

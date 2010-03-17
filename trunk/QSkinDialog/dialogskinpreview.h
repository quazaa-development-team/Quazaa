/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: dialogskinpreview.h
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : Header for the preview dialog.
 ********************************************************************************************************/
#ifndef DIALOGSKINPREVIEW_H
#define DIALOGSKINPREVIEW_H

#include <QtGui>
#include <QPoint>

namespace Ui {
	class DialogSkinPreview;
}

class DialogSkinPreview : public QDialog {
	Q_OBJECT
public:
	DialogSkinPreview(bool sizable = true, bool closable = true, QWidget *parent = 0);
	~DialogSkinPreview();
	void addChildWidget(QWidget *parent = 0);

public slots:
	void loadSkin(QString file = "");

protected:
	void changeEvent(QEvent *e);

private:
	Ui::DialogSkinPreview *ui;
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
	void on_windowFrameTopSpacer_customContextMenuRequested(QPoint pos);
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
	void setWindowTitle(const QString &);
	void setWindowIcon(const QIcon &icon);
};

#endif // DIALOGSKINPREVIEW_H

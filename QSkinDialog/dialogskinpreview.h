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

namespace Ui
{
	class DialogSkinPreview;
}

class DialogSkinPreview : public QDialog
{
	Q_OBJECT
public:
	DialogSkinPreview(QWidget* parent = 0);
	~DialogSkinPreview();

public slots:
	void loadSkin(QString file = "");

protected:
	void changeEvent(QEvent* e);

private:
	Ui::DialogSkinPreview* ui;
};

#endif // DIALOGSKINPREVIEW_H

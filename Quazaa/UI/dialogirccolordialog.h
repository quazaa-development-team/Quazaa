/*
** dialogirccolordialog.h
**
** Copyright © Quazaa Development Team, 2009-2012.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef DIALOGIRCCOLORDIALOG_H
#define DIALOGIRCCOLORDIALOG_H

#include <QDialog>

namespace Ui {
    class DialogIRCColorDialog;
}

class DialogIRCColorDialog : public QDialog
{
    Q_OBJECT

public:
	explicit DialogIRCColorDialog(QColor initialColor, QWidget *parent = 0);
	~DialogIRCColorDialog();
	QColor m_oIRCColor;

private slots:
	void on_toolButtonWhite_clicked();

	void on_toolButtonBlack_clicked();

	void on_toolButtonNavy_clicked();

	void on_toolButtonGreen_clicked();

	void on_toolButtonGray_clicked();

	void on_toolButtonLightGray_clicked();

	void on_toolButtonBlue_clicked();

	void on_toolButtonDarkCyan_clicked();

	void on_toolButtonCyan_clicked();

	void on_toolButtonPurple_clicked();

	void on_toolButtonMagenta_clicked();

	void on_toolButtonLime_clicked();

	void on_toolButtonRed_clicked();

	void on_toolButtonYellow_clicked();

	void on_toolButtonBrown_clicked();

	void on_toolButtonCancel_clicked();

	void on_toolButtonOlive_clicked();

private:
    Ui::DialogIRCColorDialog *ui;
};

#endif // DIALOGIRCCOLORDIALOG_H

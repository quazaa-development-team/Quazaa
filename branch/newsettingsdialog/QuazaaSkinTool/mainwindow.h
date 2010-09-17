//
// mainwindow.h
//
// Copyright  Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include "qtgradientmanager.h"
#include "widgetextendeditems.h"
#include "widgetnavigation.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::MainWindow *ui;
	void insertCssProperty(const QString &name, const QString &value);
	QtGradientManager *gradientManager;
	WidgetExtendedItems *pageExtendedItems;
	WidgetNavigation *pageNavigation;
	QAction *m_addImageAction;
	QAction *m_addGradientAction;
	QAction *m_addColorAction;
	QAction *m_addFontAction;
	bool isMainWindow;
	QString currentSelectionText;
	bool saved;
	QColor colorInformation;
	QColor colorSecurity;
	QColor colorNotice;
	QColor colorDebug;
	QColor colorWarning;
	QColor colorError;
	QColor colorCritical;

private slots:
 void on_checkBoxBoldListsHighlighted_clicked(bool checked);
 void on_toolButtonColorListsHighlighted_clicked();
 void on_checkBoxBoldListsSpecial_clicked(bool checked);
 void on_toolButtonColorListsSpecial_clicked();
 void on_checkBoxBoldListsActive_clicked(bool checked);
 void on_toolButtonColorListsActive_clicked();
 void on_checkBoxBoldListsNormal_clicked(bool checked);
 void on_toolButtonColorListsNormal_clicked();
 void on_checkBoxCriticalBold_clicked(bool checked);
 void on_checkBoxErrorBold_clicked(bool checked);
 void on_checkBoxWarningBold_clicked(bool checked);
 void on_checkBoxDebugBold_clicked(bool checked);
 void on_checkBoxNoticeBold_clicked(bool checked);
 void on_checkBoxSecurityBold_clicked(bool checked);
 void on_checkBoxInformationBold_clicked(bool checked);
 void on_toolButtonColorCritical_clicked();
 void on_toolButtonColorError_clicked();
 void on_toolButtonColorWarning_clicked();
 void on_toolButtonColorDebug_clicked();
 void on_toolButtonColorNotice_clicked();
 void on_toolButtonColorSecurity_clicked();
 void on_toolButtonColorInformation_clicked();
	void applyIcon();
	void on_plainTextEditDescription_textChanged();
	void on_lineEditVersion_textChanged(QString );
	void on_spinBoxMainIconSize_valueChanged(int value);
	void on_checkBoxMainIconVisible_toggled(bool checked);
	void on_plainTextEditStyleSheet_textChanged();
	void applySheets();
	void on_actionAbout_Quazaa_Skin_Tool_triggered();
	void on_actionSkin_Creation_Guide_triggered();
	void on_actionPaste_triggered();
	void on_actionCopy_triggered();
	void on_actionCut_triggered();
	void on_actionPackage_For_Distribution_triggered();
	void on_actionExit_triggered();
	void on_actionSave_triggered();
	void on_actionClose_triggered();
	void on_actionOpen_triggered();
	void on_treeWidgetSelector_itemClicked(QTreeWidgetItem* item, int column);
	void validateStyleSheet();
	bool isStyleSheetValid(const QString &styleSheet);
	void slotContextMenuRequested();
	void slotAddImage(const QString &property);
	void slotAddGradient(const QString &property);
	void slotAddColor(const QString &property);
	void slotAddFont();
	void on_actionNew_triggered();
	void enableEditing(bool enable);
	void skinChangeEvent();
	void updateWindowStyleSheet(bool mainWindw);
	void closeEvent(QCloseEvent *e);
	void updateLogPreview();
};

#endif // MAINWINDOW_H

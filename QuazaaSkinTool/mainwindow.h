#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QTreeWidgetItem>
#include "qtgradientmanager.h"

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
	QActionGroup *actionGroupMainNavigation;
	void insertCssProperty(const QString &name, const QString &value);
	QtGradientManager *gradientManager;
	QAction *m_addImageAction;
	QAction *m_addGradientAction;
	QAction *m_addColorAction;
	QAction *m_addFontAction;
	bool isMainWindow;
	QString currentSelectionText;
	bool saved;

private slots:
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
};

#endif // MAINWINDOW_H

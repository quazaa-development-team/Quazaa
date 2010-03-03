#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>

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

private slots:
	void on_actionNavigation_triggered();
	void on_actionExtended_Items_triggered();
	void on_actionStandard_Items_triggered();
	void on_actionWindow_Frame_triggered();
	void on_actionSplash_Screen_triggered();
};

#endif // MAINWINDOW_H

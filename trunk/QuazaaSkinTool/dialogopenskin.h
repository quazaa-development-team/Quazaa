#ifndef DIALOGOPENSKIN_H
#define DIALOGOPENSKIN_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
	class DialogOpenSkin;
}

class DialogOpenSkin : public QDialog {
	Q_OBJECT
public:
	DialogOpenSkin(QWidget *parent = 0);
	~DialogOpenSkin();
	QString skinFile;

protected:
	void changeEvent(QEvent *e);

private:
	Ui::DialogOpenSkin *ui;

private slots:
	void on_listWidgetSkins_itemClicked(QListWidgetItem* item);
	void on_pushButtonOK_clicked();
	void on_pushButtonCancel_clicked();
};

#endif // DIALOGOPENSKIN_H

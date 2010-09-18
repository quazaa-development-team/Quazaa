#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QtGui/QDialog>

namespace Ui {
	class DialogSettings;
}

class DialogSettings : public QDialog
{
	Q_OBJECT

public:
	explicit DialogSettings(QWidget *parent = 0);
	~DialogSettings();

public slots:
	void switchSettingsPage(int page);

protected:
	void changeEvent(QEvent *e);

private:
	Ui::DialogSettings *ui;
};

#endif // DIALOGSETTINGS_H

#ifndef DIALOGIRCSETTINGS_H
#define DIALOGIRCSETTINGS_H

#include <QDialog>
#include <QWizardPage>

#include "generalwizardpage.h"
#include "colorswizardpage.h"
#include "messageswizardpage.h"

namespace Ui {
class DialogIrcSettings;
}

class DialogIrcSettings : public QDialog
{
	Q_OBJECT

public:
	explicit DialogIrcSettings(QWidget *parent = 0);
	~DialogIrcSettings();

private:
	Ui::DialogIrcSettings *ui;
	GeneralWizardPage *generalPage;
	MessagesWizardPage *messagesPage;
	ColorsWizardPage *colorsPage;

public slots:
	void saveSettings();
	void loadSettings();

private slots:
	void on_pushButtonOK_clicked();
	void enableApply();
};

#endif // DIALOGIRCSETTINGS_H

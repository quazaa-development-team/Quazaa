#ifndef DIALOGIRCSETTINGS_H
#define DIALOGIRCSETTINGS_H

#include <QDialog>
#include <QWizardPage>

namespace ColorColumns {
	enum Columns {
		Message,
		Color
	};
}

namespace ShortcutColumns {
	enum Columns {
		Description,
		Shortcut
	};
}

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

public slots:
	void saveSettings();
	void loadSettings();

private slots:
	void on_pushButtonOK_clicked();
	void enableApply();
};

#endif // DIALOGIRCSETTINGS_H

#ifndef WIZARDQUICKSTART_H
#define WIZARDQUICKSTART_H

#include <QWizard>

namespace Ui {
    class WizardQuickStart;
}

class WizardQuickStart : public QWizard
{
    Q_OBJECT

public:
    explicit WizardQuickStart(QWidget *parent = 0);
    ~WizardQuickStart();

protected:
	virtual void changeEvent(QEvent* e);

private:
    Ui::WizardQuickStart *ui;

private slots:
	void on_pushButtonSharesRemove_clicked();
	void on_pushButtonSharesAdd_clicked();
	void accept();
};

#endif // WIZARDQUICKSTART_H

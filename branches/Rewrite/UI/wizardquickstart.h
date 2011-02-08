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

private:
    Ui::WizardQuickStart *ui;
};

#endif // WIZARDQUICKSTART_H

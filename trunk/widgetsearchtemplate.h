#ifndef WIDGETSEARCHTEMPLATE_H
#define WIDGETSEARCHTEMPLATE_H

#include <QtGui/QDialog>

namespace Ui {
    class WidgetSearchTemplate;
}

class WidgetSearchTemplate : public QWidget {
    Q_OBJECT
public:
    WidgetSearchTemplate(QWidget *parent = 0);
    ~WidgetSearchTemplate();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSearchTemplate *m_ui;
};

#endif // WIDGETSEARCHTEMPLATE_H

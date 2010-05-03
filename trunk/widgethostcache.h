#ifndef WIDGETHOSTCACHE_H
#define WIDGETHOSTCACHE_H

#include <QMainWindow>

namespace Ui {
    class WidgetHostCache;
}

class WidgetHostCache : public QMainWindow {
    Q_OBJECT
public:
    WidgetHostCache(QWidget *parent = 0);
    ~WidgetHostCache();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetHostCache *ui;
};

#endif // WIDGETHOSTCACHE_H

#ifndef WINMAIN_H
#define WINMAIN_H

#include <QMainWindow>

namespace Ui {
    class WinMain;
}

class WinMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit WinMain(QWidget *parent = 0);
    ~WinMain();

private:
    Ui::WinMain *ui;
};

#endif // WINMAIN_H

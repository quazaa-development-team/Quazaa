#ifndef QUAZAASYSINFO_H
#define QUAZAASYSINFO_H

#include <QObject>

namespace OSVersion {
    enum OSVersion{ Linux, Unix, BSD, MacCheetah, MacPuma, MacJaguar, MacPanther, MacTiger, MacLeopard, MacSnowLeopard, Win2000, WinXP, Win2003, WinVista, Win7 };
}

class QuazaaSysInfo : public QObject
{
    Q_OBJECT
public:
    explicit QuazaaSysInfo(QObject *parent = 0);
    static OSVersion::OSVersion osVersion();
    static QString osVersionToString();

signals:

public slots:

};

#endif // QUAZAASYSINFO_H

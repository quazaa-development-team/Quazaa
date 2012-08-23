#include "quazaasysinfo.h"

#include <QProcess>
#include <QFile>
#include <QDebug>

QuazaaSysInfo::QuazaaSysInfo(QObject *parent) :
    QObject(parent)
{
}
OSVersion::OSVersion QuazaaSysInfo::osVersion()
{
#ifdef Q_OS_LINUX
    return OSVersion::Linux;
#endif
#ifdef Q_OS_FREEBSD
    return OSVersion::BSD;
#endif
#ifdef Q_OS_NETBSD
    return OSVersion::BSD;
#endif
#ifdef Q_OS_OPENBSD
    return OSVersion::BSD;
#endif
#ifdef Q_OS_UNIX
    return OSVersion::Unix;
#endif
#ifdef Q_OS_MAC
    switch ( QSysInfo::MacintoshVersion )
    {
    case QSysInfo::MV_CHEETAH:
        return OSVersion::MacCheetah;
    case QSysInfo::MV_PUMA:
        return OSVersion::MacPuma;
    case QSysInfo::MV_JAGUAR:
        return OSVersion::MacJaguar;
    case QSysInfo::MV_PANTHER:
        return OSVersion::MacPanther;
    case QSysInfo::MV_TIGER:
        return OSVersion::MacTiger;
    case QSysInfo::MV_LEOPARD:
        return OSVersion::MacLeopard;
    case QSysInfo::MV_SNOWLEOPARD:
        return OSVersion::MacSnowLeopard;
    default:
        return OSVersion::MacSnowLeopard;
    }
#endif
#ifdef Q_OS_WIN
    switch ( QSysInfo::windowsVersion() )
    {
    case QSysInfo::WV_2000:
        return OSVersion::Win2000;
    case QSysInfo::WV_XP:
        return OSVersion::WinXP;
    case QSysInfo::WV_2003:
        return OSVersion::Win2003;
    case QSysInfo::WV_VISTA:
        return OSVersion::WinVista;
    case QSysInfo::WV_WINDOWS7:
        return OSVersion::Win7;
    default:
        return OSVersion::Win7;
    }
#endif
}

QString QuazaaSysInfo::osVersionToString()
{
    QString operatingSystemString = "";

#if defined(Q_WS_WIN)
    switch ( QSysInfo::WindowsVersion ) {
        case QSysInfo::WV_32s :
            operatingSystemString = "Windows 3.1 with Win 32s";
            break;
        case QSysInfo::WV_95 :
            operatingSystemString = "Windows 95";
            break;
        case QSysInfo::WV_98 :
            operatingSystemString = "Windows 98";
            break;
        case QSysInfo::WV_Me :
            operatingSystemString = "Windows Me";
            break;
        case QSysInfo::WV_NT :
            operatingSystemString = "Windows NT (operating system version 4.0)";
            break;
        case QSysInfo::WV_2000 :
            operatingSystemString = "Windows 2000 (operating system version 5.0)";
            break;
        case QSysInfo::WV_XP :
            operatingSystemString = "Windows XP (operating system version 5.1)";
            break;
        case QSysInfo::WV_2003 :
            operatingSystemString = "Windows Server 2003, Windows Server 2003 R2, Windows Home Server, Windows XP Professional x64 Edition (operating system version 5.2)";
            break;
        case QSysInfo::WV_VISTA :
            operatingSystemString = "Windows Vista, Windows Server 2008 (operating system version 6.0)";
            break;
        case QSysInfo::WV_WINDOWS7 :
            operatingSystemString = "Windows 7 (operating system version 6.1)";
            break;
        case QSysInfo::WV_CE :
            operatingSystemString = "Windows CE";
            break;
        case QSysInfo::WV_CENET :
            operatingSystemString = "Windows CE .NET";
            break;
        case QSysInfo::WV_CE_5 :
            operatingSystemString = "Windows CE 5.x";
            break;
        case QSysInfo::WV_CE_6 :
            operatingSystemString = "Windows CE 6.x";
            break;
        default :
            operatingSystemString = "Unknown Windows operating system.";
            break;
    }
#elif defined(Q_WS_MAC)
    switch ( QSysInfo::MacintoshVersion ) {
        case QSysInfo::MV_9 :
            operatingSystemString = "Mac OS 9 (unsupported)";
            break;
        case QSysInfo::MV_10_0 :
            operatingSystemString = "Mac OS X 10.0 Cheetah (unsupported)";
            break;
        case QSysInfo::MV_10_1 :
            operatingSystemString = "Mac OS X 10.1 Puma (unsupported)";
            break;
        case QSysInfo::MV_10_2 :
            operatingSystemString = "Mac OS X 10.2 Jaguar (unsupported)";
            break;
        case QSysInfo::MV_10_3 :
            operatingSystemString = "Mac OS X 10.3 Panther";
            break;
        case QSysInfo::MV_10_4 :
            operatingSystemString = "Mac OS X 10.4 Tiger";
            break;
        case QSysInfo::MV_10_5 :
            operatingSystemString = "Mac OS X 10.5 Leopard";
            break;
        case QSysInfo::MV_10_6 :
            operatingSystemString = "Mac OS X 10.6 Snow Leopard";
            break;
        case QSysInfo::MV_Unknown :
            operatingSystemString = "An unknown and currently unsupported Mac platform";
            break;
        default :
            operatingSystemString = "Unknown Mac operating system.";
            break;
    }
#else
    //TODO: Detect Unix, Linux etc. distro as described on http://www.novell.com/coolsolutions/feature/11251.html
    operatingSystemString = "Linux";
    QProcess process;
    process.start("uname -s");
    bool result = process.waitForFinished(1000);
    QString os = process.readAllStandardOutput().trimmed();

    process.start("uname -r");
    result = process.waitForFinished(1000);
    QString rev = process.readAllStandardOutput().trimmed();

    process.start("uname -m");
    result = process.waitForFinished(1000);
    QString mach = process.readAllStandardOutput().trimmed();

    if ( os == "SunOS" ) {
        os = "Solaris";

        process.start("uname -p");
        result = process.waitForFinished(1000);
        QString arch = process.readAllStandardOutput().trimmed();

        process.start("uname -v");
        result = process.waitForFinished(1000);
        QString timestamp = process.readAllStandardOutput().trimmed();

        operatingSystemString = os + " " + rev + " (" + arch + " " + timestamp + ")";
    }
    else if ( os == "AIX" ) {
        process.start("oslevel -r");
        result = process.waitForFinished(1000);
        QString oslevel = process.readAllStandardOutput().trimmed();

        operatingSystemString = os + "oslevel " + oslevel;
    }
    else if ( os == "Linux" ) {
        QString dist;
        QString pseudoname;
        QString kernel = rev;

        if ( QFile::exists("/etc/SUSE-release") ) {
            process.start("sh -c \"cat /etc/SUSE-release | tr '\\n' ' '| sed s/VERSION.*//\"");
            result = process.waitForFinished(1000);
            dist = process.readAllStandardOutput().trimmed();

            process.start("sh -c \"cat /etc/SUSE-release | tr '\\n' ' ' | sed s/.*=\\ //\"");
            result = process.waitForFinished(1000);
            rev = process.readAllStandardOutput().trimmed();
        }
        else if ( QFile::exists("/etc/mandrake-release") ) {
            dist = "Mandrake";

            process.start("sh -c \"cat /etc/mandrake-release | sed s/.*\\(// | sed s/\\)//\"");
            result = process.waitForFinished(1000);
            pseudoname = process.readAllStandardOutput().trimmed();

            process.start("sh -c \"cat /etc/mandrake-release | sed s/.*release\\ // | sed s/\\ .*//\"");
            result = process.waitForFinished(1000);
            rev = process.readAllStandardOutput().trimmed();
        }
        else if ( QFile::exists("/etc/lsb-release") ) {
            dist = "Ubuntu";

            QString processCall = "sh -c \"cat /etc/lsb-release | grep --max-count=1 DISTRIB_RELEASE=\"";
            process.start( processCall );
            result = process.waitForFinished(1000);
            rev = process.readAllStandardOutput().trimmed();
            qDebug() << "revision:" << rev;
            if(!rev.isEmpty())
            {
                rev.remove("DISTRIB_RELEASE=");
                rev.remove("\"");
            }
            QString errorStr = process.readAllStandardError();

            process.start("sh -c \"cat /etc/lsb-release | grep --max-count=1 DISTRIB_CODENAME=\"");
            result = process.waitForFinished(1000);
            pseudoname = process.readAllStandardOutput().trimmed();
            qDebug() << "pseudoname:" << pseudoname;
            if(!pseudoname.isEmpty())
            {
                pseudoname.remove("DISTRIB_CODENAME=");
                pseudoname.remove("\"");
            }
       }
            else if ( QFile::exists("/etc/debian_version") ) {
            dist = "Debian";

            process.start("cat /etc/debian_version");
            result = process.waitForFinished(1000);
            dist += process.readAllStandardOutput().trimmed();

            rev = "";
        }

        if ( QFile::exists("/etc/UnitedLinux-release") ) {
            process.start("sh -c \"cat /etc/UnitedLinux-release | grep --max-count=1 \"VERSION = \"\"");
            result = process.waitForFinished(1000);
            dist += process.readAllStandardOutput().trimmed();
            if(!dist.isEmpty())
            {
                dist.remove("VERSION = ");
                dist.remove("\"");
            }
        }


        if ( QFile::exists("/etc/os-release") ) { //This file makes distribution identification much easier.
            process.start("sh -c \"cat /etc/os-release | grep --max-count=1 PRETTY_NAME=\"");
            result = process.waitForFinished(1000);
            QString distname = process.readAllStandardOutput().trimmed();
            if(!distname.isEmpty())
            {
                distname.remove("PRETTY_NAME=");
                distname.remove("\"");

                dist = distname;
                pseudoname = "";
            }
        }

        operatingSystemString = os;
        if(!dist.isEmpty())
            operatingSystemString.append(" " + dist);
        if(!rev.isEmpty())
            operatingSystemString.append(" " + rev);
        operatingSystemString.append(" (");
        if(!pseudoname.isEmpty())
            operatingSystemString.append(pseudoname + " ");
        if(!kernel.isEmpty())
            operatingSystemString.append(kernel + " ");
        operatingSystemString.append(mach + ")");
    }
#endif

    return operatingSystemString;
}

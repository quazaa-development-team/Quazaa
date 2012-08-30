#include "quazaasysinfo.h"

#include <QProcess>
#include <QFile>
#include <QDebug>

#ifdef Q_OS_WIN
#include <tchar.h>
#include <QSettings>

#define BUFSIZE 80

#ifndef Q_CC_MSVC
#define VER_SUITE_WH_SERVER 0x8000
#endif

//Windows product definitions
#define PRODUCT_UNDEFINED                       0x00000000
#define PRODUCT_ULTIMATE                        0x00000001
#define PRODUCT_HOME_BASIC                      0x00000002
#define PRODUCT_HOME_PREMIUM                    0x00000003
#define PRODUCT_ENTERPRISE                      0x00000004
#define PRODUCT_HOME_BASIC_N                    0x00000005
#define PRODUCT_BUSINESS                        0x00000006
#define PRODUCT_STANDARD_SERVER                 0x00000007
#define PRODUCT_DATACENTER_SERVER               0x00000008
#define PRODUCT_SMALLBUSINESS_SERVER            0x00000009
#define PRODUCT_ENTERPRISE_SERVER               0x0000000A
#define PRODUCT_STARTER                         0x0000000B
#define PRODUCT_DATACENTER_SERVER_CORE          0x0000000C
#define PRODUCT_STANDARD_SERVER_CORE            0x0000000D
#define PRODUCT_ENTERPRISE_SERVER_CORE          0x0000000E
#define PRODUCT_ENTERPRISE_SERVER_IA64          0x0000000F
#define PRODUCT_BUSINESS_N                      0x00000010
#define PRODUCT_WEB_SERVER                      0x00000011
#define PRODUCT_CLUSTER_SERVER                  0x00000012
#define PRODUCT_HOME_SERVER                     0x00000013
#define PRODUCT_STORAGE_EXPRESS_SERVER          0x00000014
#define PRODUCT_STORAGE_STANDARD_SERVER         0x00000015
#define PRODUCT_STORAGE_WORKGROUP_SERVER        0x00000016
#define PRODUCT_STORAGE_ENTERPRISE_SERVER       0x00000017
#define PRODUCT_SERVER_FOR_SMALLBUSINESS        0x00000018
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM    0x00000019

//Windows 7 additional product definitions
#define PRODUCT_HOME_PREMIUM_N 0x0000001A
#define PRODUCT_ENTERPRISE_N 0x0000001B
#define PRODUCT_ULTIMATE_N 0x0000001C
#define PRODUCT_WEB_SERVER_CORE 0x0000001D
#define PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT 0x0000001E
#define PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY 0x0000001F
#define PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING 0x00000020
#define PRODUCT_SERVER_FOUNDATION 0x00000021
#define PRODUCT_HOME_PREMIUM_SERVER 0x00000022
#define PRODUCT_SERVER_FOR_SMALLBUSINESS_V 0x00000023
#define PRODUCT_STANDARD_SERVER_V 0x00000024
#define PRODUCT_DATACENTER_SERVER_V 0x00000025
#define PRODUCT_ENTERPRISE_SERVER_V 0x00000026
#define PRODUCT_DATACENTER_SERVER_CORE_V 0x00000027
#define PRODUCT_STANDARD_SERVER_CORE_V 0x00000028
#define PRODUCT_ENTERPRISE_SERVER_CORE_V 0x00000029
#define PRODUCT_HYPERV 0x0000002A
#define PRODUCT_STORAGE_EXPRESS_SERVER_CORE 0x0000002B
#define PRODUCT_STORAGE_STANDARD_SERVER_CORE 0x0000002C
#define PRODUCT_STORAGE_WORKGROUP_SERVER_CORE 0x0000002D
#define PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE 0x0000002E
#define PRODUCT_STARTER_N 0x0000002F
#define PRODUCT_PROFESSIONAL 0x00000030
#define PRODUCT_PROFESSIONAL_N 0x00000031
#define PRODUCT_SB_SOLUTION_SERVER 0x00000032
#define PRODUCT_SERVER_FOR_SB_SOLUTIONS 0x00000033
#define PRODUCT_STANDARD_SERVER_SOLUTIONS 0x00000034
#define PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE 0x00000035
#define PRODUCT_SB_SOLUTION_SERVER_EM 0x00000036
#define PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM 0x00000037
#define PRODUCT_SOLUTION_EMBEDDEDSERVER 0x00000038
#define PRODUCT_SOLUTION_EMBEDDEDSERVER_CORE 0x00000039
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE 0x0000003F
#define PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT 0x0000003B
#define PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL 0x0000003C
#define PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC 0x0000003D
#define PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC 0x0000003E
#define PRODUCT_CLUSTER_SERVER_V 0x00000040
#define PRODUCT_EMBEDDED 0x00000041
#define PRODUCT_STARTER_E 0x00000042
#define PRODUCT_HOME_BASIC_E 0x00000043
#define PRODUCT_HOME_PREMIUM_E 0x00000044
#define PRODUCT_PROFESSIONAL_E 0x00000045
#define PRODUCT_ENTERPRISE_E 0x00000046
#define PRODUCT_ULTIMATE_E 0x00000047

#define PROCESSOR_ARCHITECTURE_AMD64 9
#define SM_SERVERR2 89

typedef void (WINAPI *PGetNativeSystemInfo)(LPSYSTEM_INFO);

typedef bool (WINAPI *PGetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);
#endif

QuazaaSysInfo::QuazaaSysInfo(QObject *parent) :
	QObject(parent)
{
#ifdef Q_OS_WIN
	bool canDetect = true;
	PGetNativeSystemInfo pGNSI = 0;

	m_bOsVersionInfoEx = false;
	m_nWindowsVersion = WindowsVersion::Windows;
	m_nWindowsEdition = WindowsEdition::EditionUnknown;
	memset(m_sServicePack, 0, sizeof(m_sServicePack));

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	ZeroMemory(&m_osvi, sizeof(OSVERSIONINFOEX));
	m_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(m_bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &m_osvi)) )
	{
		// If that fails, try using the OSVERSIONINFO structure.
		m_osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &m_osvi) )
		canDetect = FALSE;
	}

	pGNSI = (PGetNativeSystemInfo) GetProcAddress(
		GetModuleHandle(L"kernel32.dll"),
		"GetNativeSystemInfo");

	if(0 != pGNSI) pGNSI(&m_SysInfo);
	else GetSystemInfo(&m_SysInfo);

	if(canDetect)
	{
		DetectWindowsVersion();
		DetectWindowsEdition();
		DetectWindowsServicePack();
	}
#endif
}

QuazaaSysInfo::~QuazaaSysInfo()
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
	QString sVersion;
	QString sEdition;
	QString sServicePack;
	QString sMachine;

	switch(GetWindowsVersion())
	{
	case WindowsVersion::Windows:
		sVersion = "Windows";
		break;
	case WindowsVersion::Windows32s:
		sVersion = "Windows 32s";
		break;
	case WindowsVersion::Windows95:
		sVersion = "Windows 95";
		break;
	case WindowsVersion::Windows95OSR2:
		sVersion = "Windows 95 SR2";
		break;
	case WindowsVersion::Windows98:
		sVersion = "Windows 98";
		break;
	case WindowsVersion::Windows98SE:
		sVersion = "Windows 98 SE";
		break;
	case WindowsVersion::WindowsMillennium:
		sVersion = "Windows Me";
		break;
	case WindowsVersion::WindowsNT351:
		sVersion = "Windows NT 3.51";
		break;
	case WindowsVersion::WindowsNT40:
		sVersion = "Windows NT 4.0";
		break;
	case WindowsVersion::WindowsNT40Server:
		sVersion = "Windows NT 4.0 Server";
		break;
	case WindowsVersion::Windows2000:
		sVersion = "Windows 2000";
		break;
	case WindowsVersion::WindowsXP:
		sVersion = "Windows XP";
		break;
	case WindowsVersion::WindowsXPProfessionalx64:
		sVersion = "Windows XP Professional x64";
		break;
	case WindowsVersion::WindowsHomeServer:
		sVersion = "Windows Home Server";
		break;
	case WindowsVersion::WindowsServer2003:
		sVersion = "Windows Server 2003";
		break;
	case WindowsVersion::WindowsServer2003R2:
		sVersion = "Windows Server 2003 R2";
		break;
	case WindowsVersion::WindowsVista:
		sVersion = "Windows Vista";
		break;
	case WindowsVersion::WindowsServer2008:
		sVersion = "Windows Server 2008";
		break;
	case WindowsVersion::WindowsServer2008R2:
		sVersion = "Windows Server 2008 R2";
		break;
	case WindowsVersion::Windows7:
		sVersion = "Windows 7";
		break;
	default:
		sVersion = "Unknown Windows Version";
		break;
	}

	switch(GetWindowsEdition())
	{
	case WindowsEdition::EditionUnknown:
		sEdition = "Unknown Edition";
		break;
	case WindowsEdition::Workstation:
		sEdition = "Workstation";
		break;
	case WindowsEdition::Server:
		sEdition = "Server";
		break;
	case WindowsEdition::AdvancedServer:
		sEdition = "Advanced Server";
		break;
	case WindowsEdition::Home:
		sEdition = "Home";
		break;
	case WindowsEdition::Ultimate:
		sEdition = "Ultimate";
		break;
	case WindowsEdition::HomeBasic:
		sEdition = "Home Basic";
		break;
	case WindowsEdition::HomePremium:
		sEdition = "Home Premium";
		break;
	case WindowsEdition::Enterprise:
		sEdition = "Enterprise";
		break;
	case WindowsEdition::HomeBasic_N:
		sEdition = "Home Basic N";
		break;
	case WindowsEdition::Business:
		sEdition = "Business";
		break;
	case WindowsEdition::StandardServer:
		sEdition = "Standard Server";
		break;
	case WindowsEdition::EnterpriseServerCore:
		sEdition = "Enterprise Server Core";
		break;
	case WindowsEdition::EnterpriseServerIA64:
		sEdition = "Enterprise Server IA64";
		break;
	case WindowsEdition::Business_N:
		sEdition = "Business N";
		break;
	case WindowsEdition::WebServer:
		sEdition = "Web Server";
		break;
	case WindowsEdition::ClusterServer:
		sEdition = "Cluster Server";
		break;
	case WindowsEdition::HomeServer:
		sEdition = "Home Server";
		break;
	case WindowsEdition::StorageExpressServer:
		sEdition = "Storage Express Server";
		break;
	case WindowsEdition::StorageStandardServer:
		sEdition = "Storage Standard Server";
		break;
	case WindowsEdition::StorageWorkgroupServer:
		sEdition = "Storage Workgroup Server";
		break;
	case WindowsEdition::StorageEnterpriseServer:
		sEdition = "Storage Enterprise Server";
		break;
	case WindowsEdition::ServerForSmallBusiness:
		sEdition = "Server For Small Business";
		break;
	case WindowsEdition::SmallBusinessServerPremium:
		sEdition = "Small Business Server Premium";
		break;
	case WindowsEdition::HomePremium_N:
		sEdition = "Home Premium N";
		break;
	case WindowsEdition::Enterprise_N:
		sEdition = "Enterprise N";
		break;
	case WindowsEdition::Ultimate_N:
		sEdition = "Ultimate N";
		break;
	case WindowsEdition::WebServerCore:
		sEdition = "Web Server Core";
		break;
	case WindowsEdition::MediumBusinessServerManagement:
		sEdition = "Medium Business Server Management";
		break;
	case WindowsEdition::MediumBusinessServerSecurity:
		sEdition = "Medium Business Server Security";
		break;
	case WindowsEdition::MediumBusinessServerMessaging:
		sEdition = "Medium Business Server Messaging";
		break;
	case WindowsEdition::ServerFoundation:
		sEdition = "Server Foundation";
		break;
	case WindowsEdition::HomePremiumServer:
		sEdition = "Home Premium Server";
		break;
	case WindowsEdition::ServerForSmallBusiness_V:
		sEdition = "Server For Small Business V";
		break;
	case WindowsEdition::StandardServer_V:
		sEdition = "Standard Server V";
		break;
	case WindowsEdition::DatacenterServer_V:
		sEdition = "Datacenter Server V";
		break;
	case WindowsEdition::EnterpriseServer_V:
		sEdition = "Enterprise Server V";
		break;
	case WindowsEdition::DatacenterServerCore_V:
		sEdition = "Datacenter Server Core V";
		break;
	case WindowsEdition::StandardServerCore_V:
		sEdition = "Standard Server Core V";
		break;
	case WindowsEdition::EnterpriseServerCore_V:
		sEdition = "Enterprise Server Core V";
		break;
	case WindowsEdition::HyperV:
		sEdition = "Hyper V";
		break;
	case WindowsEdition::StorageExpressServerCore:
		sEdition = "Storage Express Server Core";
		break;
	case WindowsEdition::StorageStandardServerCore:
		sEdition = "Storage Standard Server Core";
		break;
	case WindowsEdition::StorageWorkgroupServerCore:
		sEdition = "Storage Workgroup Server Core";
		break;
	case WindowsEdition::StorageEnterpriseServerCore:
		sEdition = "Storage Enterprise Server Core";
		break;
	case WindowsEdition::Starter_N:
		sEdition = "Starter N";
		break;
	case WindowsEdition::Professional:
		sEdition = "Professional";
		break;
	case WindowsEdition::Professional_N:
		sEdition = "Professional N";
		break;
	case WindowsEdition::SBSolutionServer:
		sEdition = "Small Business Solution Server";
		break;
	case WindowsEdition::ServerForSBSolution:
		sEdition = "Server For Small Business Solution";
		break;
	case WindowsEdition::StandardServerSolutions:
		sEdition = "Standard Server Solutions";
		break;
	case WindowsEdition::StandardServerSolutionsCore:
		sEdition = "Standard Server Solutions Core";
		break;
	case WindowsEdition::SBSolutionServer_EM:
		sEdition = "Small Business Solution Server EM";
		break;
	case WindowsEdition::ServerForSBSolution_EM:
		sEdition = "Server For Small Business Solution EM";
		break;
	case WindowsEdition::SolutionEmbeddedServer:
		sEdition = "Solution Embedded Server";
		break;
	case WindowsEdition::SolutionEmbeddedServerCore:
		sEdition = "Solution Embedded Server Core";
		break;
	case WindowsEdition::SmallBusinessServerPremiumCore:
		sEdition = "Small Business Server Premium Core";
		break;
	case WindowsEdition::EssentialBusinessServerMGMT:
		sEdition = "Essential Business Server Management";
		break;
	case WindowsEdition::EssentialBusinessServerADDL:
		sEdition = "Essential Business Server Additional";
		break;
	case WindowsEdition::EssentialBusinessServerMGMTSVC:
		sEdition = "Essential Business Server Management Service";
		break;
	case WindowsEdition::EssentialBusinessServerADDLSVC:
		sEdition = "Essential Business Server Additional Service";
		break;
	case WindowsEdition::ClusterServer_V:
		sEdition = "Cluster Server";
		break;
	case WindowsEdition::Embedded:
		sEdition = "Embedded Standard";
		break;
	case WindowsEdition::Starter_E:
		sEdition = "Starter";
		break;
	case WindowsEdition::HomeBasic_E:
		sEdition = "Home Basic";
		break;
	case WindowsEdition::HomePremium_E:
		sEdition = "Home Premium";
		break;
	case WindowsEdition::Professional_E:
		sEdition = "Professional";
		break;
	case WindowsEdition::Enterprise_E:
		sEdition = "Enterprise";
		break;
	case WindowsEdition::Ultimate_E:
		sEdition = "Ultimate";
		break;
	default:
		sEdition = "";
	}

	sServicePack = GetServicePackInfo();

	if(Is64bitPlatform())
		sMachine = "64-bit";
	else
		sMachine = "32-bit";

	operatingSystemString = QString(sVersion + " ");
	if(!sEdition.isEmpty())
		operatingSystemString.append(sEdition + " ");
	if(!sServicePack.isEmpty())
		operatingSystemString.append(tr("with %1 ").arg(sServicePack));
	operatingSystemString.append(sMachine);

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

#ifdef Q_OS_WIN
void QuazaaSysInfo::DetectWindowsVersion()
{
   if(m_bOsVersionInfoEx)
   {
	  switch (m_osvi.dwPlatformId)
	  {
	  case VER_PLATFORM_WIN32s:
		 {
			m_nWindowsVersion = WindowsVersion::Windows32s;
		 }
		 break;

		 // Test for the Windows 95 product family.
	  case VER_PLATFORM_WIN32_WINDOWS:
		 {
			switch(m_osvi.dwMajorVersion)
			{
			case 4:
			   {
				  switch(m_osvi.dwMinorVersion)
				  {
				  case 0:
					 if (m_osvi.szCSDVersion[0] == 'B' || m_osvi.szCSDVersion[0] == 'C')
						m_nWindowsVersion = WindowsVersion::Windows95OSR2;
					 else
						m_nWindowsVersion = WindowsVersion::Windows95;
					 break;
				  case 10:
					 if (m_osvi.szCSDVersion[0] == 'A')
						m_nWindowsVersion = WindowsVersion::Windows98SE;
					 else
						m_nWindowsVersion = WindowsVersion::Windows98;
					 break;
				  case 90:
					 m_nWindowsVersion = WindowsVersion::WindowsMillennium;
					 break;
				  }
			   }
			   break;
			}
		 }
		 break;

		 // Test for the Windows NT product family.
	  case VER_PLATFORM_WIN32_NT:
		 {
			switch (m_osvi.dwMajorVersion)
			{
			case 3:
			   m_nWindowsVersion = WindowsVersion::WindowsNT351;
			   break;

			case 4:
			   switch (m_osvi.wProductType)
			   {
			   case 1:
				  m_nWindowsVersion = WindowsVersion::WindowsNT40;
				  break;
			   case 3:
				  m_nWindowsVersion = WindowsVersion::WindowsNT40Server;
				  break;
			   }
			   break;

			case 5:
			   {
				  switch (m_osvi.dwMinorVersion)
				  {
				  case 0:
					 m_nWindowsVersion = WindowsVersion::Windows2000;
					 break;
				  case 1:
					 m_nWindowsVersion = WindowsVersion::WindowsXP;
					 break;
				  case 2:
					 {
						if (m_osvi.wSuiteMask == VER_SUITE_WH_SERVER)
						{
						   m_nWindowsVersion = WindowsVersion::WindowsHomeServer;
						}
						else if (m_osvi.wProductType == VER_NT_WORKSTATION &&
						   m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
						{
						   m_nWindowsVersion = WindowsVersion::WindowsXPProfessionalx64;
						}
						else
						{
						   m_nWindowsVersion = ::GetSystemMetrics(SM_SERVERR2) == 0 ?
						   WindowsVersion::WindowsServer2003 :
						   WindowsVersion::WindowsServer2003R2;
						}
					 }
					 break;
				  }

			   }
			   break;

			case 6:
			   {
				  switch (m_osvi.dwMinorVersion)
				  {
				  case 0:
					 {
						m_nWindowsVersion = m_osvi.wProductType == VER_NT_WORKSTATION ?
						WindowsVersion::WindowsVista :
						WindowsVersion::WindowsServer2008;
					 }
					 break;

				  case 1:
					 {
						m_nWindowsVersion = m_osvi.wProductType == VER_NT_WORKSTATION ?
						WindowsVersion::Windows7 :
						WindowsVersion::WindowsServer2008R2;
					 }
					 break;
				  }
			   }
			   break;
			}
		 }
		 break;
	  }
   }
   else // Test for specific product on Windows NT 4.0 SP5 and earlier
   {
	  QSettings registry("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\ProductOptions",QSettings::NativeFormat);
	  QString sProductType;


	  sProductType = registry.value("ProductType").toString();
	  sProductType = sProductType.toUpper();

	  if ( sProductType == "WINNT" )
	  {
		 if ( m_osvi.dwMajorVersion <= 4 )
		 {
			m_nWindowsVersion = WindowsVersion::WindowsNT40;
			m_nWindowsEdition = WindowsEdition::Workstation;
		 }
	  }
	  if ( sProductType == "LANMANNT" )
	  {
		 if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 2 )
		 {
			m_nWindowsVersion = WindowsVersion::WindowsServer2003;
		 }

		 if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 0 )
		 {
			m_nWindowsVersion = WindowsVersion::Windows2000;
			m_nWindowsEdition = WindowsEdition::Server;
		 }

		 if ( m_osvi.dwMajorVersion <= 4 )
		 {
			m_nWindowsVersion = WindowsVersion::WindowsNT40;
			m_nWindowsEdition = WindowsEdition::Server;
		 }
	  }
	  if ( sProductType == "SERVERNT" )
	  {
		 if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 2 )
		 {
			m_nWindowsVersion = WindowsVersion::WindowsServer2003;
			m_nWindowsEdition = WindowsEdition::EnterpriseServer;
		 }

		 if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 0 )
		 {
			m_nWindowsVersion = WindowsVersion::Windows2000;
			m_nWindowsEdition = WindowsEdition::AdvancedServer;
		 }

		 if ( m_osvi.dwMajorVersion <= 4 )
		 {
			m_nWindowsVersion = WindowsVersion::WindowsNT40;
			m_nWindowsEdition = WindowsEdition::EnterpriseServer;
		 }
	  }
   }
}

void QuazaaSysInfo::DetectWindowsEdition()
{
   if(m_bOsVersionInfoEx)
   {
	  switch(m_osvi.dwMajorVersion)
	  {
	  case 4:
		 {
			switch(m_osvi.wProductType)
			{
			case VER_NT_WORKSTATION:
			   m_nWindowsEdition = WindowsEdition::Workstation;
			   break;

			case VER_NT_SERVER:
			   m_nWindowsEdition = (m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE) != 0 ?
				  WindowsEdition::EnterpriseServer :
				  WindowsEdition::StandardServer;
			   break;
			}
		 }
		 break;

	  case 5:
		 {
			switch (m_osvi.wProductType)
			{
			case VER_NT_WORKSTATION:
			   {
				  m_nWindowsEdition = (m_osvi.wSuiteMask & VER_SUITE_PERSONAL) != 0 ?
					 WindowsEdition::Home :
					 WindowsEdition::Professional;
			   }
			   break;

			case VER_NT_SERVER:
			   {
				  switch(m_osvi.dwMinorVersion)
				  {
				  case 0:
					 {
						if ((m_osvi.wSuiteMask & VER_SUITE_DATACENTER) != 0)
						{
						   m_nWindowsEdition = WindowsEdition::DatacenterServer;
						}
						else if ((m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE) != 0)
						{
						   m_nWindowsEdition = WindowsEdition::AdvancedServer;
						}
						else
						{
						   m_nWindowsEdition = WindowsEdition::Server;
						}
					 }
					 break;

				  default:
					 {
						if ((m_osvi.wSuiteMask & VER_SUITE_DATACENTER) != 0)
						{
						   m_nWindowsEdition = WindowsEdition::DatacenterServer;
						}
						else if ((m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE) != 0)
						{
						   m_nWindowsEdition = WindowsEdition::EnterpriseServer;
						}
						else if ((m_osvi.wSuiteMask & VER_SUITE_BLADE) != 0)
						{
						   m_nWindowsEdition = WindowsEdition::WebServer;
						}
						else
						{
						   m_nWindowsEdition = WindowsEdition::StandardServer;
						}
					 }
					 break;
				  }
			   }
			   break;
			}
		 }
		 break;

	  case 6:
		 {
			DWORD dwReturnedProductType = DetectProductInfo();
			switch (dwReturnedProductType)
			{
			case PRODUCT_UNDEFINED:
			   m_nWindowsEdition = WindowsEdition::EditionUnknown;
			   break;

			case PRODUCT_ULTIMATE:
			   m_nWindowsEdition = WindowsEdition::Ultimate;
			   break;
			case PRODUCT_HOME_BASIC:
			   m_nWindowsEdition = WindowsEdition::HomeBasic;
			   break;
			case PRODUCT_HOME_PREMIUM:
			   m_nWindowsEdition = WindowsEdition::HomePremium;
			   break;
			case PRODUCT_ENTERPRISE:
			   m_nWindowsEdition = WindowsEdition::Enterprise;
			   break;
			case PRODUCT_HOME_BASIC_N:
			   m_nWindowsEdition = WindowsEdition::HomeBasic_N;
			   break;
			case PRODUCT_BUSINESS:
			   m_nWindowsEdition = WindowsEdition::Business;
			   break;
			case PRODUCT_STANDARD_SERVER:
			   m_nWindowsEdition = WindowsEdition::StandardServer;
			   break;
			case PRODUCT_DATACENTER_SERVER:
			   m_nWindowsEdition = WindowsEdition::DatacenterServer;
			   break;
			case PRODUCT_SMALLBUSINESS_SERVER:
			   m_nWindowsEdition = WindowsEdition::SmallBusinessServer;
			   break;
			case PRODUCT_ENTERPRISE_SERVER:
			   m_nWindowsEdition = WindowsEdition::EnterpriseServer;
			   break;
			case PRODUCT_STARTER:
			   m_nWindowsEdition = WindowsEdition::Starter;
			   break;
			case PRODUCT_DATACENTER_SERVER_CORE:
			   m_nWindowsEdition = WindowsEdition::DatacenterServerCore;
			   break;
			case PRODUCT_STANDARD_SERVER_CORE:
			   m_nWindowsEdition = WindowsEdition::StandardServerCore;
			   break;
			case PRODUCT_ENTERPRISE_SERVER_CORE:
			   m_nWindowsEdition = WindowsEdition::EnterpriseServerCore;
			   break;
			case PRODUCT_ENTERPRISE_SERVER_IA64:
			   m_nWindowsEdition = WindowsEdition::EnterpriseServerIA64;
			   break;
			case PRODUCT_BUSINESS_N:
			   m_nWindowsEdition = WindowsEdition::Business_N;
			   break;
			case PRODUCT_WEB_SERVER:
			   m_nWindowsEdition = WindowsEdition::WebServer;
			   break;
			case PRODUCT_CLUSTER_SERVER:
			   m_nWindowsEdition = WindowsEdition::ClusterServer;
			   break;
			case PRODUCT_HOME_SERVER:
			   m_nWindowsEdition = WindowsEdition::HomeServer;
			   break;
			case PRODUCT_STORAGE_EXPRESS_SERVER:
			   m_nWindowsEdition = WindowsEdition::StorageExpressServer;
			   break;
			case PRODUCT_STORAGE_STANDARD_SERVER:
			   m_nWindowsEdition = WindowsEdition::StorageStandardServer;
			   break;
			case PRODUCT_STORAGE_WORKGROUP_SERVER:
			   m_nWindowsEdition = WindowsEdition::StorageWorkgroupServer;
			   break;
			case PRODUCT_STORAGE_ENTERPRISE_SERVER:
			   m_nWindowsEdition = WindowsEdition::StorageEnterpriseServer;
			   break;
			case PRODUCT_SERVER_FOR_SMALLBUSINESS:
			   m_nWindowsEdition = WindowsEdition::ServerForSmallBusiness;
			   break;
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
			   m_nWindowsEdition = WindowsEdition::SmallBusinessServerPremium;
			   break;

			if (QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS7)
			{
				case PRODUCT_HOME_PREMIUM_N:
					m_nWindowsEdition = WindowsEdition::HomePremium_N;
					break;
				case PRODUCT_ENTERPRISE_N:
					m_nWindowsEdition = WindowsEdition::Enterprise_N;
					break;
				case PRODUCT_ULTIMATE_N:
					m_nWindowsEdition = WindowsEdition::Ultimate_N;
					break;
				case PRODUCT_WEB_SERVER_CORE:
					m_nWindowsEdition = WindowsEdition::WebServerCore;
					break;
				case PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT:
					m_nWindowsEdition = WindowsEdition::MediumBusinessServerManagement;
					break;
				case PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY:
					m_nWindowsEdition = WindowsEdition::MediumBusinessServerSecurity;
					break;
				case PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING:
					m_nWindowsEdition = WindowsEdition::MediumBusinessServerMessaging;
					break;
				case PRODUCT_SERVER_FOUNDATION:
					m_nWindowsEdition = WindowsEdition::ServerFoundation;
					break;
				case PRODUCT_HOME_PREMIUM_SERVER:
					m_nWindowsEdition = WindowsEdition::HomePremiumServer;
					break;
				case PRODUCT_SERVER_FOR_SMALLBUSINESS_V:
					m_nWindowsEdition = WindowsEdition::ServerForSmallBusiness_V;
					break;
				case PRODUCT_STANDARD_SERVER_V:
					m_nWindowsEdition = WindowsEdition::StandardServer_V;
					break;
				case PRODUCT_DATACENTER_SERVER_V:
					m_nWindowsEdition = WindowsEdition::DatacenterServer_V;
					break;
				case PRODUCT_ENTERPRISE_SERVER_V:
					m_nWindowsEdition = WindowsEdition::EnterpriseServer_V;
					break;
				case PRODUCT_DATACENTER_SERVER_CORE_V:
					m_nWindowsEdition = WindowsEdition::DatacenterServerCore_V;
					break;
				case PRODUCT_STANDARD_SERVER_CORE_V:
					m_nWindowsEdition = WindowsEdition::StandardServerCore_V;
					break;
				case PRODUCT_ENTERPRISE_SERVER_CORE_V:
					m_nWindowsEdition = WindowsEdition::EnterpriseServerCore_V;
					break;
				case PRODUCT_HYPERV:
					m_nWindowsEdition = WindowsEdition::HyperV;
					break;
				case PRODUCT_STORAGE_EXPRESS_SERVER_CORE:
					m_nWindowsEdition = WindowsEdition::StorageExpressServerCore;
					break;
				case PRODUCT_STORAGE_STANDARD_SERVER_CORE:
					m_nWindowsEdition = WindowsEdition::StorageStandardServerCore;
					break;
				case PRODUCT_STORAGE_WORKGROUP_SERVER_CORE:
					m_nWindowsEdition = WindowsEdition::StorageWorkgroupServerCore;
					break;
				case PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE:
					m_nWindowsEdition = WindowsEdition::StorageEnterpriseServerCore;
					break;
				case PRODUCT_STARTER_N:
					m_nWindowsEdition = WindowsEdition::Starter_N;
					break;
				case PRODUCT_PROFESSIONAL:
					m_nWindowsEdition = WindowsEdition::Professional;
					break;
				case PRODUCT_PROFESSIONAL_N:
					m_nWindowsEdition = WindowsEdition::Professional_N;
					break;
				case PRODUCT_SB_SOLUTION_SERVER:
					m_nWindowsEdition = WindowsEdition::SBSolutionServer;
					break;
				case PRODUCT_SERVER_FOR_SB_SOLUTIONS:
					m_nWindowsEdition = WindowsEdition::ServerForSBSolution;
					break;
				case PRODUCT_STANDARD_SERVER_SOLUTIONS:
					m_nWindowsEdition = WindowsEdition::StandardServerSolutions;
					break;
				case PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE:
					m_nWindowsEdition = WindowsEdition::StandardServerSolutionsCore;
					break;
				case PRODUCT_SB_SOLUTION_SERVER_EM:
					m_nWindowsEdition = WindowsEdition::SBSolutionServer_EM;
					break;
				case PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM:
					m_nWindowsEdition = WindowsEdition::ServerForSBSolution_EM;
					break;
				case PRODUCT_SOLUTION_EMBEDDEDSERVER:
					m_nWindowsEdition = WindowsEdition::SolutionEmbeddedServer;
					break;
				case PRODUCT_SOLUTION_EMBEDDEDSERVER_CORE:
					m_nWindowsEdition = WindowsEdition::SolutionEmbeddedServerCore;
					break;
				case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE:
					m_nWindowsEdition = WindowsEdition::SmallBusinessServerPremiumCore;
					break;
				case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT:
					m_nWindowsEdition = WindowsEdition::EssentialBusinessServerMGMT;
					break;
				case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL:
					m_nWindowsEdition = WindowsEdition::EssentialBusinessServerADDL;
					break;
				case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC:
					m_nWindowsEdition = WindowsEdition::EssentialBusinessServerMGMTSVC;
					break;
				case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC:
					m_nWindowsEdition = WindowsEdition::EssentialBusinessServerADDLSVC;
					break;
				case PRODUCT_CLUSTER_SERVER_V:
					m_nWindowsEdition = WindowsEdition::ClusterServer_V;
					break;
				case PRODUCT_EMBEDDED:
					m_nWindowsEdition = WindowsEdition::Embedded;
					break;
				case PRODUCT_STARTER_E:
					m_nWindowsEdition = WindowsEdition::Starter_E;
					break;
				case PRODUCT_HOME_BASIC_E:
					m_nWindowsEdition = WindowsEdition::HomeBasic_E;
					break;
				case PRODUCT_HOME_PREMIUM_E:
					m_nWindowsEdition = WindowsEdition::HomePremium_E;
					break;
				case PRODUCT_PROFESSIONAL_E:
					m_nWindowsEdition = WindowsEdition::Professional_E;
					break;
				case PRODUCT_ENTERPRISE_E:
					m_nWindowsEdition = WindowsEdition::Enterprise_E;
					break;
				case PRODUCT_ULTIMATE_E:
					m_nWindowsEdition = WindowsEdition::Ultimate_E;
					break;
				}
			}
		 }
		 break;
	  }
   }
}

void QuazaaSysInfo::DetectWindowsServicePack()
{
	// Display service pack (if any) and build number.

	if( m_osvi.dwMajorVersion == 4 &&
		   lstrcmpi( m_osvi.szCSDVersion, L"Service Pack 6" ) == 0 )
	{
		QSettings registry("HKEY_LOCAL_MACHINE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix", QSettings::NativeFormat);

		// Test for SP6 versus SP6a.
		if( registry.childGroups().contains("Q246009"))
		{
			sprintf(m_sServicePack, "Service Pack 6a (Build %lu)", m_osvi.dwBuildNumber & 0xFFFF );
		}
		else // Windows NT 4.0 prior to SP6a
		{
			char* sCSDVersion = new char[128];
			wcstombs(sCSDVersion, m_osvi.szCSDVersion, sizeof(m_osvi.szCSDVersion));
			sprintf(m_sServicePack, "%s (Build %lu)",
				sCSDVersion,
				m_osvi.dwBuildNumber & 0xFFFF);
		}
	}
	else // Windows NT 3.51 and earlier or Windows 2000 and later
	{
		char* sCSDVersion = new char[128];
		wcstombs(sCSDVersion, m_osvi.szCSDVersion, sizeof(m_osvi.szCSDVersion));
		sprintf(m_sServicePack, "%s (Build %lu)",
			sCSDVersion,
			m_osvi.dwBuildNumber & 0xFFFF);
	}
}

DWORD QuazaaSysInfo::DetectProductInfo()
{
	DWORD dwProductInfo = PRODUCT_UNDEFINED;

	if (QSysInfo::windowsVersion() >= QSysInfo::WV_VISTA)
	{
		if(m_osvi.dwMajorVersion >= 6)
		{
			PGetProductInfo lpProducInfo = (PGetProductInfo)GetProcAddress(
				GetModuleHandle(L"kernel32.dll"), "GetProductInfo");

			if(0 != lpProducInfo)
			{
				lpProducInfo(m_osvi.dwMajorVersion,
							m_osvi.dwMinorVersion,
							m_osvi.wServicePackMajor,
							m_osvi.wServicePackMinor,
							&dwProductInfo);
			}
		}
   }

   return dwProductInfo;
}

WindowsVersion::WindowsVersion QuazaaSysInfo::GetWindowsVersion() const
{
	return m_nWindowsVersion;
}

WindowsEdition::WindowsEdition QuazaaSysInfo::GetWindowsEdition() const
{
   return m_nWindowsEdition;
}

bool QuazaaSysInfo::IsNTPlatform() const
{
	return m_osvi.dwPlatformId == VER_PLATFORM_WIN32_NT;
}

bool QuazaaSysInfo::IsWindowsPlatform() const
{
	return m_osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS;
}

bool QuazaaSysInfo::IsWin32sPlatform() const
{
	return m_osvi.dwPlatformId == VER_PLATFORM_WIN32s;
}

DWORD QuazaaSysInfo::GetMajorVersion() const
{
	return m_osvi.dwMajorVersion;
}

DWORD QuazaaSysInfo::GetMinorVersion() const
{
	return m_osvi.dwMinorVersion;
}

DWORD QuazaaSysInfo::GetBuildNumber() const
{
	return m_osvi.dwBuildNumber;
}

DWORD QuazaaSysInfo::GetPlatformID() const
{
	return m_osvi.dwPlatformId;
}

QString QuazaaSysInfo::GetServicePackInfo() const
{
	QString servicePack = m_sServicePack;

	if(servicePack.isEmpty())
		return "";
	else
		return servicePack;
}

bool QuazaaSysInfo::Is32bitPlatform() const
{
	return !Is64bitPlatform();
}

bool QuazaaSysInfo::Is64bitPlatform() const
{
	return (
		m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ||
		m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64);
}
#endif //Q_OS_WIN

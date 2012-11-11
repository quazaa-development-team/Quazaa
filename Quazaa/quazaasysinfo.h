#ifndef QUAZAASYSINFO_H
#define QUAZAASYSINFO_H

#include <QObject>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif


#ifdef Q_OS_WIN
namespace WindowsVersion {
	enum WindowsVersion{ Windows, Windows32s, Windows95, Windows95OSR2, Windows98, Windows98SE, WindowsMillennium, WindowsNT351, WindowsNT40, WindowsNT40Server, Windows2000, WindowsXP, WindowsXPProfessionalx64, WindowsHomeServer, WindowsServer2003, WindowsServer2003R2, WindowsVista, WindowsServer2008, WindowsServer2008R2, Windows7};
}

namespace WindowsEdition {
	enum WindowsEdition{ EditionUnknown, Workstation, Server, AdvancedServer, Home, Ultimate, HomeBasic, HomePremium, Enterprise, HomeBasic_N, Business, StandardServer, DatacenterServer, SmallBusinessServer, EnterpriseServer, Starter, DatacenterServerCore, StandardServerCore, EnterpriseServerCore, EnterpriseServerIA64, Business_N, WebServer, ClusterServer, HomeServer, StorageExpressServer, StorageStandardServer, StorageWorkgroupServer, StorageEnterpriseServer, ServerForSmallBusiness, SmallBusinessServerPremium, HomePremium_N, Enterprise_N, Ultimate_N, WebServerCore, MediumBusinessServerManagement, MediumBusinessServerSecurity, MediumBusinessServerMessaging, ServerFoundation, HomePremiumServer, ServerForSmallBusiness_V, StandardServer_V, DatacenterServer_V, EnterpriseServer_V, DatacenterServerCore_V, StandardServerCore_V, EnterpriseServerCore_V, HyperV, StorageExpressServerCore, StorageStandardServerCore, StorageWorkgroupServerCore, StorageEnterpriseServerCore, Starter_N, Professional, Professional_N, SBSolutionServer, ServerForSBSolution, StandardServerSolutions, StandardServerSolutionsCore, SBSolutionServer_EM, ServerForSBSolution_EM, SolutionEmbeddedServer, SolutionEmbeddedServerCore, SmallBusinessServerPremiumCore, EssentialBusinessServerMGMT, EssentialBusinessServerADDL, EssentialBusinessServerMGMTSVC, EssentialBusinessServerADDLSVC, ClusterServer_V, Embedded, Starter_E, HomeBasic_E, HomePremium_E, Professional_E, Enterprise_E, Ultimate_E };
}
#endif

namespace OSVersion {
	enum OSVersion{ Linux, Unix, BSD, MacCheetah, MacPuma, MacJaguar, MacPanther, MacTiger, MacLeopard, MacSnowLeopard, Win2000, WinXP, Win2003, WinVista, Win7 };
}

class QuazaaSysInfo : public QObject
{
	Q_OBJECT
#ifdef Q_OS_WIN
	WindowsVersion::WindowsVersion	m_nWindowsVersion;
	WindowsEdition::WindowsEdition	m_nWindowsEdition;
	char							m_sServicePack[128];
	OSVERSIONINFOEX					m_osvi;
	SYSTEM_INFO						m_SysInfo;
	bool							m_bOsVersionInfoEx;

private:
	void DetectWindowsVersion();
	void DetectWindowsEdition();
	void DetectWindowsServicePack();
	DWORD DetectProductInfo();
#endif

public:
	explicit QuazaaSysInfo(QObject *parent = 0);
	virtual ~QuazaaSysInfo();

	OSVersion::OSVersion osVersion();
	QString osVersionToString();

#ifdef Q_OS_WIN
	WindowsVersion::WindowsVersion GetWindowsVersion() const;		// returns the windows version
	WindowsEdition::WindowsEdition GetWindowsEdition() const;		// returns the windows edition
	bool IsNTPlatform() const;						   // true if NT platform
	bool IsWindowsPlatform() const;					   // true is Windows platform
	bool IsWin32sPlatform() const;					   // true is Win32s platform
	DWORD GetMajorVersion() const;					   // returns major version
	DWORD GetMinorVersion() const;				   	// returns minor version
	DWORD GetBuildNumber() const;			   		// returns build number
	DWORD GetPlatformID() const;				      	// returns platform ID
	QString GetServicePackInfo() const;// additional information about service pack
	bool Is32bitPlatform() const;					   // true if platform is 32-bit
	bool Is64bitPlatform() const;					   // true if platform is 64-bit
#endif

signals:

public slots:

};

#endif // QUAZAASYSINFO_H

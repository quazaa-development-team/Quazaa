#include "randomportselector.h"
#include "commonfunctions.h"

/**
 * Note that this requires C++11 compilation to be enabled.
 * @brief common::getRandomUnusedPort
 * @return
 */
static short common::getRandomUnusedPort()
{
	if ( !oUsedPorts.size() )
	{

		/**
		 * This list contains are all ports that are known to be used by other applications.
		 * Source: http://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers (2012-08-31)
		 */
		oUsedPorts.insert(1024);	//  TCP	UDP	Reserved
		oUsedPorts.insert(1025);	//  TCP		NFS or IIS or Teradata
		oUsedPorts.insert(1026);	//  TCP		Often used by Microsoft DCOM services
		oUsedPorts.insert(1029);	//  TCP		Often used by Microsoft DCOM services
		oUsedPorts.insert(1058);	//  TCP	UDP	nim); IBM AIX Network Installation Manager (NIM)
		oUsedPorts.insert(1059);	//  TCP	UDP	nimreg); IBM AIX Network Installation Manager (NIM)
		oUsedPorts.insert(1080);	//  TCP		SOCKS proxy
		oUsedPorts.insert(1085);	//  TCP	UDP	WebObjects
		oUsedPorts.insert(1098);	//  TCP	UDP	rmiactivation); RMI Activation
		oUsedPorts.insert(1099);	//  TCP	UDP	rmiregistry); RMI Registry
		oUsedPorts.insert(1109);	//  	UDP	Reserved[1]
		oUsedPorts.insert(1109);	//  TCP		Reserved[1]
		oUsedPorts.insert(1109);	//  TCP		Kerberos Post Office Protocol (KPOP)
		oUsedPorts.insert(1110);	//  	UDP	EasyBits School network discovery protocol (for Intel's CMPC platform)
		oUsedPorts.insert(1140);	//  TCP	UDP	AutoNOC protocol
		oUsedPorts.insert(1167);	//  	UDP	phone); conference calling
		oUsedPorts.insert(1169);	//  TCP	UDP	Tripwire
		oUsedPorts.insert(1176);	//  TCP		Perceptive Automation Indigo Home automation server
		oUsedPorts.insert(1182);	//  TCP	UDP	AcceleNet Intelligent Transfer Protocol
		oUsedPorts.insert(1194);	//  TCP	UDP	OpenVPN
		oUsedPorts.insert(1198);	//  TCP	UDP	The cajo project Free dynamic transparent distributed computing in Java
		oUsedPorts.insert(1200);	//  TCP		scol); protocol used by SCOL 3D virtual worlds server to answer world name resolution client request[41]
		oUsedPorts.insert(1200);	//  	UDP	scol); protocol used by SCOL 3D virtual worlds server to answer world name resolution client request
		oUsedPorts.insert(1200);	//  	UDP	Steam Friends Applet
		oUsedPorts.insert(1214);	//  TCP		Kazaa
		oUsedPorts.insert(1217);	//  TCP		Uvora Online
		oUsedPorts.insert(1220);	//  TCP		QuickTime Streaming Server administration
		oUsedPorts.insert(1223);	//  TCP	UDP	TGP); TrulyGlobal Protocol); also known as "The Gur Protocol" (named for Gur Kimchi of TrulyGlobal)
		oUsedPorts.insert(1234);	//  	UDP	VLC media player default port for UDP/RTP stream
		oUsedPorts.insert(1236);	//  TCP		Symantec BindView Control UNIX Default port for TCP management server connections
		oUsedPorts.insert(1241);	//  TCP	UDP	Nessus Security Scanner
		oUsedPorts.insert(1270);	//  TCP	UDP	Microsoft System Center Operations Manager (SCOM) (formerly Microsoft Operations Manager (MOM)) agent
		oUsedPorts.insert(1293);	//  TCP	UDP	IPSec (Internet Protocol Security)
		oUsedPorts.insert(1301);	//  TCP		Palmer Performance OBDNet
		oUsedPorts.insert(1309);	//  TCP		Altera Quartus jtagd
		oUsedPorts.insert(1311);	//  TCP		Dell OpenManage HTTPS
		oUsedPorts.insert(1313);	//  TCP		Xbiim (Canvii server)
		oUsedPorts.insert(1314);	//  TCP		Festival Speech Synthesis System
		oUsedPorts.insert(1319);	//  TCP		AMX ICSP
		oUsedPorts.insert(1319);	//  	UDP	AMX ICSP
		oUsedPorts.insert(1337);	//  	UDP	Men and Mice DNS
		oUsedPorts.insert(1337);	//  TCP		Men and Mice DNS
		oUsedPorts.insert(1337);	//  TCP		PowerFolder P2P Encrypted File Synchronization Program
		oUsedPorts.insert(1337);	//  TCP		WASTE Encrypted File Sharing Program
		oUsedPorts.insert(1344);	//  TCP		Internet Content Adaptation Protocol
		oUsedPorts.insert(1352);	//  TCP		IBM Lotus Notes/Domino[42] (RPC) protocol
		oUsedPorts.insert(1387);	//  TCP	UDP	cadsi-lm); LMS International (formerly Computer Aided Design Software); Inc. (CADSI)) LM
		oUsedPorts.insert(1414);	//  TCP		IBM WebSphere MQ (formerly known as MQSeries)
		oUsedPorts.insert(1417);	//  TCP	UDP	Timbuktu Service 1 Port
		oUsedPorts.insert(1418);	//  TCP	UDP	Timbuktu Service 2 Port
		oUsedPorts.insert(1419);	//  TCP	UDP	Timbuktu Service 3 Port
		oUsedPorts.insert(1420);	//  TCP	UDP	Timbuktu Service 4 Port
		oUsedPorts.insert(1431);	//  TCP		Reverse Gossip Transport Protocol (RGTP)); used to access a General-purpose Reverse-Ordered Gossip Gathering System (GROGGS) bulletin board); such as that implemented on the Cambridge University's Phoenix system
		oUsedPorts.insert(1433);	//  TCP		MSSQL (Microsoft SQL Server database management system) Server
		oUsedPorts.insert(1434);	//  TCP	UDP	MSSQL (Microsoft SQL Server database management system) Monitor
		oUsedPorts.insert(1453);	//  TCP	UDP	National Port of Turkey
		oUsedPorts.insert(1470);	//  TCP		Solarwinds Kiwi Log Server
		oUsedPorts.insert(1494);	//  TCP		Citrix XenApp Independent Computing Architecture (ICA) thin client protocol
		oUsedPorts.insert(1500);	//  TCP		NetGuard GuardianPro firewall (NT4-based) Remote Management
		oUsedPorts.insert(1501);	//  	UDP	NetGuard GuardianPro firewall (NT4-based) Authentication Client
		oUsedPorts.insert(1503);	//  TCP	UDP	Windows Live Messenger (Whiteboard and Application Sharing)
		oUsedPorts.insert(1512);	//  TCP	UDP	Microsoft Windows Internet Name Service (WINS)
		oUsedPorts.insert(1513);	//  TCP	UDP	Garena Garena Gaming Client
		oUsedPorts.insert(1521);	//  TCP		nCube License Manager
		oUsedPorts.insert(1521);	//  TCP		Oracle database default listener); in future releases official port 2483
		oUsedPorts.insert(1524);	//  TCP	UDP	ingreslock); ingres
		oUsedPorts.insert(1526);	//  TCP		Oracle database common alternative for listener
		oUsedPorts.insert(1527);	//  TCP		Apache Derby Network Server default port
		oUsedPorts.insert(1533);	//  TCP		IBM Sametime IM—Virtual Places Chat Microsoft SQL Server
		oUsedPorts.insert(1534);	//  	UDP	Eclipse Target Communication Framework (TCF) agent discovery[43]
		oUsedPorts.insert(1547);	//  TCP	UDP	Laplink
		oUsedPorts.insert(1550);	//  TCP	UDP	3m-image-lm Image Storage license manager 3M Company
		oUsedPorts.insert(1550);	//  		Gadu-Gadu (direct client-to-client)
		oUsedPorts.insert(1581);	//  	UDP	MIL STD 2045-47001 VMF
		oUsedPorts.insert(1589);	//  	UDP	Cisco VQP (VLAN Query Protocol) / VMPS
		oUsedPorts.insert(1590);	//  TCP		GE Smallworld Datastore Server (SWMFS/Smallworld Master Filesystem)
		oUsedPorts.insert(1627);	//  		iSketch
		oUsedPorts.insert(1645);	//  TCP	UDP	radius auth); RADIUS authentication protocol (default for Cisco and Juniper Networks RADIUS servers); but see port 1812 below)
		oUsedPorts.insert(1646);	//  TCP	UDP	radius acct); RADIUS authentication protocol (default for Cisco and Juniper Networks RADIUS servers); but see port 1813 below)
		oUsedPorts.insert(1666);	//  TCP		Perforce
		oUsedPorts.insert(1677);	//  TCP	UDP	Novell GroupWise clients in client/server access mode
		oUsedPorts.insert(1688);	//  TCP		Microsoft Key Management Service for KMS Windows Activation
		oUsedPorts.insert(1700);	//  	UDP	Cisco RADIUS Change of Authorization for TrustSec
		oUsedPorts.insert(1701);	//  	UDP	Layer 2 Forwarding Protocol (L2F) & Layer 2 Tunneling Protocol (L2TP)
		oUsedPorts.insert(1707);	//  TCP	UDP	Windward Studios
		oUsedPorts.insert(1707);	//  TCP	Romtoc Packet Protocol (L2F) & Layer 2 Tunneling Protocol (L2TP)
		oUsedPorts.insert(1716);	//  TCP		America's Army Massively multiplayer online game (MMO)
		oUsedPorts.insert(1719);	//  	UDP	H.323 Registration and alternate communication
		oUsedPorts.insert(1720);	//  TCP		H.323 Call signalling
		oUsedPorts.insert(1723);	//  TCP	UDP	Microsoft Point-to-Point Tunneling Protocol (PPTP)
		oUsedPorts.insert(1725);	//  	UDP	Valve Steam Client
		oUsedPorts.insert(1755);	//  TCP	UDP	Microsoft Media Services (MMS); ms-streaming)
		oUsedPorts.insert(1761);	//  	UDP	cft-0
		oUsedPorts.insert(1761);	//  TCP		cft-0
		oUsedPorts.insert(1761);	//  TCP		Novell Zenworks Remote Control utility
		oUsedPorts.insert(1762);	//  Start range: cft-1 to cft-7
		oUsedPorts.insert(1763);
		oUsedPorts.insert(1764);
		oUsedPorts.insert(1765);
		oUsedPorts.insert(1766);
		oUsedPorts.insert(1767);
		oUsedPorts.insert(1768);	//  TCP	UDP	cft-1 to cft-7
		oUsedPorts.insert(1792);	//  TCP	UDP	Moby
		oUsedPorts.insert(1801);	//  TCP	UDP	Microsoft Message Queuing
		oUsedPorts.insert(1812);	//  TCP	UDP	radius); RADIUS authentication protocol
		oUsedPorts.insert(1813);	//  TCP	UDP	radacct); RADIUS accounting protocol
		oUsedPorts.insert(1863);	//  TCP		MSNP (Microsoft Notification Protocol)); used by the .NET Messenger Service and a number of Instant Messaging clients
		oUsedPorts.insert(1883);	//  TCP	UDP	MQ Telemetry Transport (MQTT)); formerly known as MQIsdp (MQSeries SCADA protocol)
		oUsedPorts.insert(1886);	//  TCP		Leonardo over IP Pro2col Ltd
		oUsedPorts.insert(1900);	//  	UDP	Microsoft SSDP Enables discovery of UPnP devices
		oUsedPorts.insert(1920);	//  TCP		IBM Tivoli monitoring console
		oUsedPorts.insert(1935);	//  TCP		Adobe Systems Macromedia Flash Real Time Messaging Protocol (RTMP) "plain" protocol
		oUsedPorts.insert(1947);	//  TCP	UDP	SentinelSRM (hasplm)); Aladdin HASP License Manager
		oUsedPorts.insert(1967);	//  	UDP	Cisco IOS IP Service Level Agreements (IP SLAs) Control Protocol
		oUsedPorts.insert(1970);	//  TCP	UDP	Netop Business Solutions Netop Remote Control
		oUsedPorts.insert(1971);	//  TCP	UDP	Netop Business Solutions Netop School
		oUsedPorts.insert(1972);	//  TCP	UDP	InterSystems Caché
		oUsedPorts.insert(1975);	//  	UDP	Cisco TCO (Documentation)
		oUsedPorts.insert(1976);	//  	UDP	Cisco TCO (Documentation)
		oUsedPorts.insert(1977);	//  	UDP	Cisco TCO (Documentation)
		oUsedPorts.insert(1984);	//  TCP		Big Brother and related Xymon (formerly Hobbit) System and Network Monitor
		oUsedPorts.insert(1985);	//  	UDP	Cisco HSRP
		oUsedPorts.insert(1994);	//  TCP	UDP	Cisco STUN-SDLC (Serial Tunneling—Synchronous Data Link Control) protocol
		oUsedPorts.insert(1997);	//  TCP		Chizmo Networks Transfer Tool
		oUsedPorts.insert(1998);	//  TCP	UDP	Cisco X.25 over TCP (XOT) service
		oUsedPorts.insert(2000);	//  TCP	UDP	Cisco SCCP (Skinny)
		oUsedPorts.insert(2001);	//  	UDP	CAPTAN Test Stand System
		oUsedPorts.insert(2002);	//  TCP		Secure Access Control Server (ACS) for Windows
		oUsedPorts.insert(2010);	//  TCP		Artemis: Spaceship Bridge Simulator default port
		oUsedPorts.insert(2030);	//  		Oracle services for Microsoft Transaction Server
		oUsedPorts.insert(2031);	//  TCP	UDP	mobrien-chat(http://chat.mobrien.com:2031)
		oUsedPorts.insert(2041);	//  TCP		Mail.Ru Agent communication protocol
		oUsedPorts.insert(2049);	//  TCP	UDP	NFS (Network File System)
		oUsedPorts.insert(2049);	//  	UDP	shilp
		oUsedPorts.insert(2053);	//  TCP		knetd Kerberos de-multiplexor
		oUsedPorts.insert(2056);	//  	UDP	Civilization 4 multiplayer
		oUsedPorts.insert(2074);	//  TCP	UDP	Vertel VMF SA (i.e. App.. SpeakFreely)
		oUsedPorts.insert(2080);	//  TCP	UDP	Autodesk NLM (FLEXlm)
		oUsedPorts.insert(2082);	//  TCP		Infowave Mobility Server
		oUsedPorts.insert(2082);	//  TCP		CPanel default
		oUsedPorts.insert(2083);	//  TCP		Secure Radius Service (radsec)
		oUsedPorts.insert(2083);	//  TCP		CPanel default SSL
		oUsedPorts.insert(2086);	//  TCP		GNUnet
		oUsedPorts.insert(2086);	//  TCP		WebHost Manager default
		oUsedPorts.insert(2087);	//  TCP		WebHost Manager default SSL
		oUsedPorts.insert(2095);	//  TCP		CPanel default Web mail
		oUsedPorts.insert(2096);	//  TCP		CPanel default SSL Web mail
		oUsedPorts.insert(2102);	//  TCP	UDP	zephyr-srv Project Athena Zephyr Notification Service server
		oUsedPorts.insert(2103);	//  TCP	UDP	zephyr-clt Project Athena Zephyr Notification Service serv-hm connection
		oUsedPorts.insert(2104);	//  TCP	UDP	zephyr-hm Project Athena Zephyr Notification Service hostmanager
		oUsedPorts.insert(2105);	//  TCP	UDP	IBM MiniPay
		oUsedPorts.insert(2105);	//  TCP	UDP	eklogin Kerberos encrypted remote login (rlogin)
		oUsedPorts.insert(2105);	//  TCP	UDP	zephyr-hm-srv Project Athena Zephyr Notification Service hm-serv connection (should use port 2102)
		oUsedPorts.insert(2121);	//  TCP		FTP proxy
		oUsedPorts.insert(2144);	//  TCP		Iron Mountain LiveVault Agent
		oUsedPorts.insert(2145);	//  TCP		Iron Mountain LiveVault Agent
		oUsedPorts.insert(2156);	//  	UDP	Talari Reliable Protocol
		oUsedPorts.insert(2160);	//  TCP		APC Agent
		oUsedPorts.insert(2161);	//  TCP		APC Agent
		oUsedPorts.insert(2181);	//  TCP	UDP	EForward-document transport system
		oUsedPorts.insert(2190);	//  	UDP	TiVoConnect Beacon
		oUsedPorts.insert(2200);	//  	UDP	Tuxanci game server[44]
		oUsedPorts.insert(2210);	//  	UDP	NOAAPORT Broadcast Network
		oUsedPorts.insert(2210);	//  TCP		NOAAPORT Broadcast Network
		oUsedPorts.insert(2210);	//  TCP		MikroTik Remote management for "The Dude"
		oUsedPorts.insert(2211);	//  	UDP	EMWIN
		oUsedPorts.insert(2211);	//  TCP		EMWIN
		oUsedPorts.insert(2211);	//  TCP		MikroTik Secure management for "The Dude"
		oUsedPorts.insert(2212);	//  	UDP	LeeCO POS Server Service
		oUsedPorts.insert(2212);	//  TCP		LeeCO POS Server Service
		oUsedPorts.insert(2212);	//  TCP		Port-A-Pour Remote WinBatch
		oUsedPorts.insert(2219);	//  TCP	UDP	NetIQ NCAP Protocol
		oUsedPorts.insert(2220);	//  TCP	UDP	NetIQ End2End
		oUsedPorts.insert(2221);	//  TCP		ESET Anti-virus updates
		oUsedPorts.insert(2222);	//  TCP		DirectAdmin default & ESET Remote Administration
		oUsedPorts.insert(2223);	//  	UDP	Microsoft Office OS X antipiracy network monitor
		oUsedPorts.insert(2261);	//  TCP	UDP	CoMotion Master
		oUsedPorts.insert(2262);	//  TCP	UDP	CoMotion Backup
		oUsedPorts.insert(2301);	//  TCP		HP System Management Redirect to port 2381
		oUsedPorts.insert(2302);	//  	UDP	ArmA multiplayer (default for game)
		oUsedPorts.insert(2302);	//  	UDP	Halo: Combat Evolved multiplayer
		oUsedPorts.insert(2303);	//  	UDP	ArmA multiplayer (default for server reporting) (default port for game +1)
		oUsedPorts.insert(2305);	//  	UDP	ArmA multiplayer (default for VoN) (default port for game +3)
		oUsedPorts.insert(2323);	//  TCP		Philips TVs based on jointSPACE [45]
		oUsedPorts.insert(2369);	//  TCP		Default for BMC Software Control-M/Server—Configuration Agent); though often changed during installation
		oUsedPorts.insert(2370);	//  TCP		Default for BMC Software Control-M/Server—to allow the Control-M/Enterprise Manager to connect to the Control-M/Server); though often changed during installation
		oUsedPorts.insert(2379);	//  TCP		KGS Go Server
		oUsedPorts.insert(2381);	//  TCP		HP Insight Manager default for Web server
		oUsedPorts.insert(2399);	//  TCP		FileMaker Data Access Layer (ODBC/JDBC)
		oUsedPorts.insert(2401);	//  TCP		CVS version control system
		oUsedPorts.insert(2404);	//  TCP		IEC 60870-5 -104); used to send electric power telecontrol messages between two systems via directly connected data circuits
		oUsedPorts.insert(2420);	//  	UDP	Westell Remote Access
		oUsedPorts.insert(2424);	//  TCP		OrientDB database listening for Binary client connections
		oUsedPorts.insert(2427);	//  	UDP	Cisco MGCP
		oUsedPorts.insert(2447);	//  TCP	UDP	ovwdb—OpenView Network Node Manager (NNM) daemon
		oUsedPorts.insert(2463);	//  TCP	UDP	LSI RAID Management formerly Symbios Logic
		oUsedPorts.insert(2480);	//  TCP		OrientDB database listening for HTTP client connections
		oUsedPorts.insert(2483);	//  TCP	UDP	Oracle database listening for unsecure client connections to the listener); replaces port 1521
		oUsedPorts.insert(2484);	//  TCP	UDP	Oracle database listening for SSL client connections to the listener
		oUsedPorts.insert(2500);	//  TCP		THEÒSMESSENGER listening for TheòsMessenger client connections
		oUsedPorts.insert(2501);	//  TCP		TheosNet-Admin listening for TheòsMessenger client connections
		oUsedPorts.insert(2518);	//  TCP	UDP	Willy
		oUsedPorts.insert(2525);	//  TCP		SMTP alternate
		oUsedPorts.insert(2535);	//  TCP		MADCAP - Multicast Address Dynamic Client Allocation Protocol
		oUsedPorts.insert(2546);	//  TCP	UDP	EVault data protection services
		oUsedPorts.insert(2593);	//  TCP	UDP	RunUO—Ultima Online server
		oUsedPorts.insert(2598);	//  TCP		new ICA (Citrix) —when Session Reliability is enabled); TCP port 2598 replaces port 1494
		oUsedPorts.insert(2599);	//  TCP		SonicWALL anti-spam traffic between Remote Analyzer (RA) and Control Center (CC)
		oUsedPorts.insert(2610);	//  TCP		Dark Ages (video game)
		oUsedPorts.insert(2612);	//  TCP	UDP	QPasa from MQSoftware
		oUsedPorts.insert(2636);	//  TCP		Solve Service
		oUsedPorts.insert(2638);	//  TCP		Sybase database listener
		oUsedPorts.insert(2641);	//  TCP	UDP	HDL Server from CNRI
		oUsedPorts.insert(2642);	//  TCP	UDP	Tragic
		oUsedPorts.insert(2698);	//  TCP	UDP	Citel / MCK IVPIP
		oUsedPorts.insert(2700);	//  Start range: KnowShowGo P2P
		oUsedPorts.insert(1701);
		oUsedPorts.insert(1702);
		oUsedPorts.insert(1703);
		oUsedPorts.insert(1704);
		oUsedPorts.insert(1705);
		oUsedPorts.insert(1706);
		oUsedPorts.insert(1707);
		oUsedPorts.insert(1708);
		oUsedPorts.insert(1709);
		oUsedPorts.insert(1710);
		oUsedPorts.insert(1711);
		oUsedPorts.insert(1712);
		oUsedPorts.insert(1713);
		oUsedPorts.insert(1714);
		oUsedPorts.insert(1715);
		oUsedPorts.insert(1716);
		oUsedPorts.insert(1717);
		oUsedPorts.insert(1718);
		oUsedPorts.insert(1719);
		oUsedPorts.insert(1720);
		oUsedPorts.insert(1721);
		oUsedPorts.insert(1722);
		oUsedPorts.insert(1723);
		oUsedPorts.insert(1724);
		oUsedPorts.insert(1725);
		oUsedPorts.insert(1726);
		oUsedPorts.insert(1727);
		oUsedPorts.insert(1728);
		oUsedPorts.insert(1729);
		oUsedPorts.insert(1730);
		oUsedPorts.insert(1731);
		oUsedPorts.insert(1732);
		oUsedPorts.insert(1733);
		oUsedPorts.insert(1734);
		oUsedPorts.insert(1735);
		oUsedPorts.insert(1736);
		oUsedPorts.insert(1737);
		oUsedPorts.insert(1738);
		oUsedPorts.insert(1739);
		oUsedPorts.insert(1740);
		oUsedPorts.insert(1741);
		oUsedPorts.insert(1742);
		oUsedPorts.insert(1743);
		oUsedPorts.insert(1744);
		oUsedPorts.insert(1745);
		oUsedPorts.insert(1746);
		oUsedPorts.insert(1747);
		oUsedPorts.insert(1748);
		oUsedPorts.insert(1749);
		oUsedPorts.insert(1750);
		oUsedPorts.insert(1751);
		oUsedPorts.insert(1752);
		oUsedPorts.insert(1753);
		oUsedPorts.insert(1754);
		oUsedPorts.insert(1755);
		oUsedPorts.insert(1756);
		oUsedPorts.insert(1757);
		oUsedPorts.insert(1758);
		oUsedPorts.insert(1759);
		oUsedPorts.insert(1760);
		oUsedPorts.insert(1761);
		oUsedPorts.insert(1762);
		oUsedPorts.insert(1763);
		oUsedPorts.insert(1764);
		oUsedPorts.insert(1765);
		oUsedPorts.insert(1766);
		oUsedPorts.insert(1767);
		oUsedPorts.insert(1768);
		oUsedPorts.insert(1769);
		oUsedPorts.insert(1770);
		oUsedPorts.insert(1771);
		oUsedPorts.insert(1772);
		oUsedPorts.insert(1773);
		oUsedPorts.insert(1774);
		oUsedPorts.insert(1775);
		oUsedPorts.insert(1776);
		oUsedPorts.insert(1777);
		oUsedPorts.insert(1778);
		oUsedPorts.insert(1779);
		oUsedPorts.insert(1780);
		oUsedPorts.insert(1781);
		oUsedPorts.insert(1782);
		oUsedPorts.insert(1783);
		oUsedPorts.insert(1784);
		oUsedPorts.insert(1785);
		oUsedPorts.insert(1786);
		oUsedPorts.insert(1787);
		oUsedPorts.insert(1788);
		oUsedPorts.insert(1789);
		oUsedPorts.insert(1790);
		oUsedPorts.insert(1791);
		oUsedPorts.insert(1792);
		oUsedPorts.insert(1793);
		oUsedPorts.insert(1794);
		oUsedPorts.insert(1795);
		oUsedPorts.insert(1796);
		oUsedPorts.insert(1797);
		oUsedPorts.insert(1798);
		oUsedPorts.insert(1799);
		oUsedPorts.insert(2800);	//  TCP		KnowShowGo P2P
		oUsedPorts.insert(2710);	//  TCP		XBT Tracker
		oUsedPorts.insert(2710);	//  	UDP	XBT Tracker experimental UDP tracker extension
		oUsedPorts.insert(2710);	//  TCP		Knuddels.de
		oUsedPorts.insert(2735);	//  TCP	UDP	NetIQ Monitor Console
		oUsedPorts.insert(2809);	//  TCP		corbaloc:iiop URL); per the CORBA 3.0.3 specification
		oUsedPorts.insert(2809);	//  TCP		IBM WebSphere Application Server (WAS) Bootstrap/rmi default
		oUsedPorts.insert(2809);	//  	UDP	corbaloc:iiop URL); per the CORBA 3.0.3 specification.
		oUsedPorts.insert(2827);	//  TCP		I2P Basic Open Bridge API
		oUsedPorts.insert(2868);	//  TCP	UDP	Norman Proprietary Event Protocol NPEP
		oUsedPorts.insert(2944);	//  	UDP	Megaco text H.248
		oUsedPorts.insert(2945);	//  	UDP	Megaco binary (ASN.1) H.248
		oUsedPorts.insert(2947);	//  TCP		gpsd GPS daemon
		oUsedPorts.insert(2948);	//  TCP	UDP	WAP-push Multimedia Messaging Service (MMS)
		oUsedPorts.insert(2949);	//  TCP	UDP	WAP-pushsecure Multimedia Messaging Service (MMS)
		oUsedPorts.insert(2967);	//  TCP		Symantec AntiVirus Corporate Edition
		oUsedPorts.insert(3000);	//  TCP		Miralix License server
		oUsedPorts.insert(3000);	//  TCP		Cloud9 Integrated Development Environment server
		oUsedPorts.insert(3000);	//  	UDP	Distributed Interactive Simulation (DIS)); modifiable default
		oUsedPorts.insert(3000);	//  TCP		Ruby on Rails development default[46]
		oUsedPorts.insert(3001);	//  TCP		Miralix Phone Monitor
		oUsedPorts.insert(3001);	//  TCP		Opsware server (Satellite)
		oUsedPorts.insert(3002);	//  TCP		Miralix CSTA
		oUsedPorts.insert(3003);	//  TCP		Miralix GreenBox API
		oUsedPorts.insert(3004);	//  TCP		Miralix InfoLink
		oUsedPorts.insert(3005);	//  TCP		Miralix TimeOut
		oUsedPorts.insert(3006);	//  TCP		Miralix SMS Client Connector
		oUsedPorts.insert(3007);	//  TCP		Miralix OM Server
		oUsedPorts.insert(3008);	//  TCP		Miralix Proxy
		oUsedPorts.insert(3017);	//  TCP		Miralix IVR and Voicemail
		oUsedPorts.insert(3025);	//  TCP		netpd.org
		oUsedPorts.insert(3030);	//  TCP	UDP	NetPanzer
		oUsedPorts.insert(3050);	//  TCP	UDP	gds_db (Interbase/Firebird)
		oUsedPorts.insert(3051);	//  TCP	UDP	Galaxy Server (Gateway Ticketing Systems)
		oUsedPorts.insert(3052);	//  TCP	UDP	APC PowerChute Network [1]
		oUsedPorts.insert(3074);	//  TCP	UDP	Xbox LIVE and/or Games for Windows - LIVE
		oUsedPorts.insert(3100);	//  TCP		SMAUSA OpCon Scheduler as the default listen port
		oUsedPorts.insert(3101);	//  TCP		BlackBerry Enterprise Server communication to cloud
		oUsedPorts.insert(3119);	//  TCP		D2000 Entis/Actis Application server
		oUsedPorts.insert(3128);	//  TCP		Web caches and the default for the Squid (software)
		oUsedPorts.insert(3128);	//  TCP		Tatsoft default client connection
		oUsedPorts.insert(3162);	//  TCP	UDP	SFLM (Standard Floating License Manager)
		oUsedPorts.insert(3225);	//  TCP	UDP	FCIP (Fiber Channel over Internet Protocol)
		oUsedPorts.insert(3233);	//  TCP	UDP	WhiskerControl research control protocol
		oUsedPorts.insert(3235);	//  TCP	UDP	Galaxy Network Service (Gateway Ticketing Systems)
		oUsedPorts.insert(3260);	//  TCP		iSCSI target
		oUsedPorts.insert(3268);	//  TCP	UDP	msft-gc); Microsoft Global Catalog (LDAP service which contains data from Active Directory forests)
		oUsedPorts.insert(3269);	//  TCP	UDP	msft-gc-ssl); Microsoft Global Catalog over SSL (similar to port 3268); LDAP over SSL)
		oUsedPorts.insert(3283);	//  TCP		Apple Remote Desktop reporting (officially Net Assistant); referring to an earlier product)
		oUsedPorts.insert(3299);	//  TCP		SAP-Router (routing application proxy for SAP R/3)
		oUsedPorts.insert(3300);	//  TCP	UDP	Debate Gopher backend database system
		oUsedPorts.insert(3305);	//  TCP	UDP	odette-ftp); Odette File Transfer Protocol (OFTP)
		oUsedPorts.insert(3306);	//  TCP	UDP	MySQL database system
		oUsedPorts.insert(3313);	//  TCP		Verisys file integrity monitoring software
		oUsedPorts.insert(3333);	//  TCP		Network Caller ID server
		oUsedPorts.insert(3333);	//  TCP		CruiseControl.rb[47]
		oUsedPorts.insert(3386);	//  TCP	UDP	GTP' 3GPP GSM/UMTS CDR logging protocol
		oUsedPorts.insert(3389);	//  TCP	UDP	Microsoft Terminal Server (RDP) officially registered as Windows Based Terminal (WBT) - Link
		oUsedPorts.insert(3396);	//  TCP	UDP	Novell NDPS Printer Agent
		oUsedPorts.insert(3412);	//  TCP	UDP	xmlBlaster
		oUsedPorts.insert(3455);	//  TCP	UDP	[RSVP] Reservation Protocol
		oUsedPorts.insert(3423);	//  TCP		Xware xTrm Communication Protocol
		oUsedPorts.insert(3424);	//  TCP		Xware xTrm Communication Protocol over SSL
		oUsedPorts.insert(3478);	//  TCP	UDP	STUN); a protocol for NAT traversal[48]
		oUsedPorts.insert(3478);	//  TCP	UDP	TURN); a protocol for NAT traversal[49]
		oUsedPorts.insert(3483);	//  	UDP	Slim Devices discovery protocol
		oUsedPorts.insert(3483);	//  TCP		Slim Devices SlimProto protocol
		oUsedPorts.insert(3516);	//  TCP	UDP	Smartcard Port
		oUsedPorts.insert(3527);	//  	UDP	Microsoft Message Queuing
		oUsedPorts.insert(3535);	//  TCP		SMTP alternate[50]
		oUsedPorts.insert(3537);	//  TCP	UDP	ni-visa-remote
		oUsedPorts.insert(3544);	//  	UDP	Teredo tunneling
		oUsedPorts.insert(3605);	//  	UDP	ComCam IO Port
		oUsedPorts.insert(3606);	//  TCP	UDP	Splitlock Server
		oUsedPorts.insert(3632);	//  TCP		distributed compiler
		oUsedPorts.insert(3689);	//  TCP		Digital Audio Access Protocol (DAAP)—used by Apple’s iTunes and AirPort Express
		oUsedPorts.insert(3690);	//  TCP	UDP	Subversion (SVN) version control system
		oUsedPorts.insert(3702);	//  TCP	UDP	Web Services Dynamic Discovery (WS-Discovery)); used by various components of Windows Vista
		oUsedPorts.insert(3723);	//  TCP	UDP	Used by many Battle.net Blizzard games (Diablo II); Warcraft II); Warcraft III); StarCraft)
		oUsedPorts.insert(3724);	//  TCP		World of Warcraft Online gaming MMORPG
		oUsedPorts.insert(3724);	//  TCP		Club Penguin Disney online game for kids
		oUsedPorts.insert(3724);	//  	UDP	World of Warcraft Online gaming MMORPG
		oUsedPorts.insert(3784);	//  TCP	UDP	VoIP program used by Ventrilo
		oUsedPorts.insert(3785);	//  	UDP	VoIP program used by Ventrilo
		oUsedPorts.insert(3799);	//  	UDP	RADIUS change of authorization
		oUsedPorts.insert(3800);	//  TCP		Used by HGG programs
		oUsedPorts.insert(3825);	//  TCP		Used by RedSeal Networks client/server connection
		oUsedPorts.insert(3826);	//  TCP		Used by RedSeal Networks client/server connection
		oUsedPorts.insert(3835);	//  TCP		Used by RedSeal Networks client/server connection
		oUsedPorts.insert(3880);	//  TCP	UDP	IGRS
		oUsedPorts.insert(3868);	//  TCP	SCTP	Diameter base protocol (RFC 3588)
		oUsedPorts.insert(3872);	//  TCP		Oracle Management Remote Agent
		oUsedPorts.insert(3899);	//  TCP		Remote Administrator
		oUsedPorts.insert(3900);	//  TCP		udt_os); IBM UniData UDT OS[51]
		oUsedPorts.insert(3945);	//  TCP	UDP	EMCADS service); a Giritech product used by G/On
		oUsedPorts.insert(3978);	//  TCP	UDP	OpenTTD game (masterserver and content service)
		oUsedPorts.insert(3979);	//  TCP	UDP	OpenTTD game
		oUsedPorts.insert(3999);	//  TCP	UDP	Norman distributed scanning service
		oUsedPorts.insert(4000);	//  TCP	UDP	Diablo II game
		oUsedPorts.insert(4001);	//  TCP		Microsoft Ants game
		oUsedPorts.insert(4007);	//  TCP		PrintBuzzer printer monitoring socket server
		oUsedPorts.insert(4018);	//  TCP	UDP	protocol information and warnings
		oUsedPorts.insert(4035);	//  TCP);	//  TCP	IBM Rational Developer for System z Remote System Explorer Daemon
		oUsedPorts.insert(4045);	//  TCP	UDP	Solaris lockd NFS lock daemon/manager
		oUsedPorts.insert(4069);	//  	UDP	Minger Email Address Verification Protocol[52]
		oUsedPorts.insert(4089);	//  TCP	UDP	OpenCORE Remote Control Service
		oUsedPorts.insert(4093);	//  TCP	UDP	PxPlus Client server interface ProvideX
		oUsedPorts.insert(4096);	//  TCP	UDP	Ascom Timeplex BRE (Bridge Relay Element)
		oUsedPorts.insert(4100);	//  		WatchGuard authentication applet default
		oUsedPorts.insert(4111);	//  TCP		Xgrid
		oUsedPorts.insert(4116);	//  TCP	UDP	Smartcard-TLS
		oUsedPorts.insert(4125);	//  TCP		Microsoft Remote Web Workplace administration
		oUsedPorts.insert(4172);	//  TCP	UDP	Teradici PCoIP
		oUsedPorts.insert(4201);	//  TCP		TinyMUD and various derivatives
		oUsedPorts.insert(4226);	//  TCP	UDP	Aleph One (game)
		oUsedPorts.insert(4224);	//  TCP		Cisco Audio Session Tunneling
		oUsedPorts.insert(4242);	//  TCP		Reverse Battle Tetris
		oUsedPorts.insert(4321);	//  TCP		Referral Whois (RWhois) Protocol[53]
		oUsedPorts.insert(4323);	//  	UDP	Lincoln Electric's ArcLink/XT
		oUsedPorts.insert(4433);	//  TCP		Axence nVision
		oUsedPorts.insert(4434);	//  TCP		Axence nVision
		oUsedPorts.insert(4435);	//  TCP		Axence nVision
		oUsedPorts.insert(4436);	//  TCP		Axence nVision
		oUsedPorts.insert(4444);	//  TCP		I2P HTTP/S proxy
		oUsedPorts.insert(4445);	//  TCP		I2P HTTP/S proxy
		oUsedPorts.insert(4486);	//  TCP	UDP	Integrated Client Message Service (ICMS)
		oUsedPorts.insert(4500);	//  	UDP	IPSec NAT Traversal (RFC 3947)
		oUsedPorts.insert(4502);	// Start range: Microsoft Silverlight[...]
		oUsedPorts.insert(4503);
		oUsedPorts.insert(4504);
		oUsedPorts.insert(4505);
		oUsedPorts.insert(4506);
		oUsedPorts.insert(4507);
		oUsedPorts.insert(4508);
		oUsedPorts.insert(4509);
		oUsedPorts.insert(4510);
		oUsedPorts.insert(4511);
		oUsedPorts.insert(4512);
		oUsedPorts.insert(4513);
		oUsedPorts.insert(4514);
		oUsedPorts.insert(4515);
		oUsedPorts.insert(4516);
		oUsedPorts.insert(4517);
		oUsedPorts.insert(4518);
		oUsedPorts.insert(4519);
		oUsedPorts.insert(4520);
		oUsedPorts.insert(4521);
		oUsedPorts.insert(4522);
		oUsedPorts.insert(4523);
		oUsedPorts.insert(4524);
		oUsedPorts.insert(4525);
		oUsedPorts.insert(4526);
		oUsedPorts.insert(4527);
		oUsedPorts.insert(4528);
		oUsedPorts.insert(4529);
		oUsedPorts.insert(4530);
		oUsedPorts.insert(4531);
		oUsedPorts.insert(4532);
		oUsedPorts.insert(4533);
		oUsedPorts.insert(4534);	//  TCP		Microsoft Silverlight connectable ports under non-elevated trust
		oUsedPorts.insert(4534);	//  	UDP	Armagetron Advanced default server port
		oUsedPorts.insert(4567);	//  TCP		Sinatra default server port in development mode (HTTP)
		oUsedPorts.insert(4569);	//  	UDP	Inter-Asterisk eXchange (IAX2)
		oUsedPorts.insert(4610);	// Start range: QualiSystems TestShell Suite Services
		oUsedPorts.insert(4611);
		oUsedPorts.insert(4612);
		oUsedPorts.insert(4613);
		oUsedPorts.insert(4614);
		oUsedPorts.insert(4615);
		oUsedPorts.insert(4616);
		oUsedPorts.insert(4617);
		oUsedPorts.insert(4618);
		oUsedPorts.insert(4619);
		oUsedPorts.insert(4620);
		oUsedPorts.insert(4621);
		oUsedPorts.insert(4622);
		oUsedPorts.insert(4623);
		oUsedPorts.insert(4624);
		oUsedPorts.insert(4625);
		oUsedPorts.insert(4626);
		oUsedPorts.insert(4627);
		oUsedPorts.insert(4628);
		oUsedPorts.insert(4629);
		oUsedPorts.insert(4630);
		oUsedPorts.insert(4631);
		oUsedPorts.insert(4632);
		oUsedPorts.insert(4633);
		oUsedPorts.insert(4634);
		oUsedPorts.insert(4635);
		oUsedPorts.insert(4636);
		oUsedPorts.insert(4637);
		oUsedPorts.insert(4638);
		oUsedPorts.insert(4639);
		oUsedPorts.insert(4640);	//  TCP		QualiSystems TestShell Suite Services
		oUsedPorts.insert(4662);	//  	UDP	OrbitNet Message Service
		oUsedPorts.insert(4662);	//  TCP		OrbitNet Message Service
		oUsedPorts.insert(4662);	//  TCP		Default for older versions of eMule[54]
		oUsedPorts.insert(4664);	//  TCP		Google Desktop Search
		oUsedPorts.insert(4672);	//  	UDP	Default for older versions of eMule[54]
		oUsedPorts.insert(4711);	//  TCP		eMule optional web interface[54]
		oUsedPorts.insert(4711);	//  TCP		McAfee Web Gateway 7 - Default GUI Port HTTP
		oUsedPorts.insert(4712);	//  TCP		McAfee Web Gateway 7 - Default GUI Port HTTPS
		oUsedPorts.insert(4713);	//  TCP		PulseAudio sound server
		oUsedPorts.insert(4728);	//  TCP		Computer Associates Desktop and Server Management (DMP)/Port Multiplexer [55]
		oUsedPorts.insert(4732);	//  	UDP	Digital Airways' OHM server's commands to mobile devices (used mainly for binary SMS)
		oUsedPorts.insert(4747);	//  TCP		Apprentice
		oUsedPorts.insert(4750);	//  TCP		BladeLogic Agent
		oUsedPorts.insert(4840);	//  TCP	UDP	OPC UA TCP Protocol for OPC Unified Architecture from OPC Foundation
		oUsedPorts.insert(4843);	//  TCP	UDP	OPC UA TCP Protocol over TLS/SSL for OPC Unified Architecture from OPC Foundation
		oUsedPorts.insert(4847);	//  TCP	UDP	Web Fresh Communication); Quadrion Software & Odorless Entertainment
		oUsedPorts.insert(4894);	//  TCP	UDP	LysKOM Protocol A
		oUsedPorts.insert(4899);	//  TCP	UDP	Radmin remote administration tool
		oUsedPorts.insert(4949);	//  TCP		Munin Resource Monitoring Tool
		oUsedPorts.insert(4950);	//  TCP	UDP	Cylon Controls UC32 Communications Port
		oUsedPorts.insert(4982);	//  TCP	UDP	Solar Data Log (JK client app for PV solar inverters)
		oUsedPorts.insert(4993);	//  TCP	UDP	Home FTP Server web Interface Default Port
		oUsedPorts.insert(5000);	//  TCP		commplex-main
		oUsedPorts.insert(5000);	//  TCP		UPnP—Windows network device interoperability
		oUsedPorts.insert(5000);	//  TCP		VTun—VPN Software
		oUsedPorts.insert(5000);	//  	UDP	FlightGear multiplayer[56]
		oUsedPorts.insert(5000);	//  	UDP	VTun—VPN Software
		oUsedPorts.insert(5001);	//  TCP		commplex-link
		oUsedPorts.insert(5001);	//  TCP		Slingbox and Slingplayer
		oUsedPorts.insert(5001);	//  TCP		Iperf (Tool for measuring TCP and UDP bandwidth performance)
		oUsedPorts.insert(5001);	//  	UDP	Iperf (Tool for measuring TCP and UDP bandwidth performance)
		oUsedPorts.insert(5002);	//  TCP		SOLICARD ARX[57]
		oUsedPorts.insert(5002);	//  	UDP	Drobo Dashboard[58]
		oUsedPorts.insert(5003);	//  TCP	UDP	FileMaker
		oUsedPorts.insert(5004);	//  TCP	UDP	RTP (Real-time Transport Protocol) media data (RFC 3551); RFC 4571)
		oUsedPorts.insert(5004);	// DCCP		RTP (Real-time Transport Protocol) media data (RFC 3551); RFC 4571)
		oUsedPorts.insert(5005);	//  TCP	UDP	RTP (Real-time Transport Protocol) control protocol (RFC 3551); RFC 4571)
		oUsedPorts.insert(5005);	// DCCP		RTP (Real-time Transport Protocol) control protocol (RFC 3551); RFC 4571)
		oUsedPorts.insert(5010);	//  TCP	UDP	Registered to: TelePath (the IBM FlowMark workflow-management system messaging platform)[59]
								//  		The TCP port is now used for: IBM WebSphere MQ Workflow
		oUsedPorts.insert(5011);	//  TCP	UDP	TelePath (the IBM FlowMark workflow-management system messaging platform)[59]
		oUsedPorts.insert(5029);	//  TCP		Sonic Robo Blast 2 : Multiplayer[60]
		oUsedPorts.insert(5031);	//  TCP	UDP	AVM CAPI-over-TCP (ISDN over Ethernet tunneling)
		oUsedPorts.insert(5037);	//  TCP		Android ADB server
		oUsedPorts.insert(5050);	//  TCP		Yahoo! Messenger
		oUsedPorts.insert(5051);	//  TCP		ita-agent Symantec Intruder Alert[61]
		oUsedPorts.insert(5060);	//  TCP	UDP	Session Initiation Protocol (SIP)
		oUsedPorts.insert(5061);	//  TCP		Session Initiation Protocol (SIP) over TLS
		oUsedPorts.insert(5070);	//  TCP		Binary Floor Control Protocol (BFCP));[62] published as RFC 4582); is a protocol that allows for an additional video channel (known as the content channel) alongside the main video channel in a video-conferencing call that uses SIP. Also used for Session Initiation Protocol (SIP) preferred port for PUBLISH on SIP Trunk to Cisco Unified Presence Server (CUPS)
		oUsedPorts.insert(5082);	//  TCP	UDP	Qpur Communication Protocol
		oUsedPorts.insert(5083);	//  TCP	UDP	Qpur File Protocol
		oUsedPorts.insert(5084);	//  TCP	UDP	EPCglobal Low Level Reader Protocol (LLRP)
		oUsedPorts.insert(5085);	//  TCP	UDP	EPCglobal Low Level Reader Protocol (LLRP) over TLS
		oUsedPorts.insert(5093);	//  	UDP	SafeNet); Inc Sentinel LM); Sentinel RMS); License Manager); Client-to-Server
		oUsedPorts.insert(5099);	//  TCP	UDP	SafeNet); Inc Sentinel LM); Sentinel RMS); License Manager); Server-to-Server
		oUsedPorts.insert(5104);	//  TCP		IBM Tivoli Framework NetCOOL/Impact[63] HTTP Service
		oUsedPorts.insert(5106);	//  TCP		A-Talk Common connection
		oUsedPorts.insert(5107);	//  TCP		A-Talk Remote server connection
		oUsedPorts.insert(5108);	//  TCP		VPOP3 Mail Server Webmail
		oUsedPorts.insert(5109);	//  TCP	UDP	VPOP3 Mail Server Status
		oUsedPorts.insert(5110);	//  TCP		ProRat Server
		oUsedPorts.insert(5121);	//  TCP		Neverwinter Nights
		oUsedPorts.insert(5150);	//  TCP	UDP	ATMP Ascend Tunnel Management Protocol[64]
		oUsedPorts.insert(5150);	//  TCP	UDP	Malware Cerberus RAT
		oUsedPorts.insert(5151);	//  TCP		ESRI SDE Instance
		oUsedPorts.insert(5151);	//  	UDP	ESRI SDE Remote Start
		oUsedPorts.insert(5154);	//  TCP	UDP	BZFlag
		oUsedPorts.insert(5176);	//  TCP		ConsoleWorks default UI interface
		oUsedPorts.insert(5190);	//  TCP		ICQ and AOL Instant Messenger
		oUsedPorts.insert(5222);	//  TCP		Extensible Messaging and Presence Protocol (XMPP) client connection[65][66]
		oUsedPorts.insert(5223);	//  TCP		Extensible Messaging and Presence Protocol (XMPP) client connection over SSL
		oUsedPorts.insert(5228);	//  TCP		HP Virtual Room Service
		oUsedPorts.insert(5228);	//  TCP		Android Market
		oUsedPorts.insert(5246);	//  	UDP	Control And Provisioning of Wireless Access Points (CAPWAP) CAPWAP control[67]
		oUsedPorts.insert(5247);	//  	UDP	Control And Provisioning of Wireless Access Points (CAPWAP) CAPWAP data[67]
		oUsedPorts.insert(5269);	//  TCP		Extensible Messaging and Presence Protocol (XMPP) server connection[65][66]
		oUsedPorts.insert(5280);	//  TCP		Extensible Messaging and Presence Protocol (XMPP) XEP-0124: Bidirectional-streams Over Synchronous HTTP (BOSH)
		oUsedPorts.insert(5281);	//  TCP		Undo License Manager
		oUsedPorts.insert(5281);	//  TCP		Extensible Messaging and Presence Protocol (XMPP)[68]
		oUsedPorts.insert(5298);	//  TCP	UDP	Extensible Messaging and Presence Protocol (XMPP)[69]
		oUsedPorts.insert(5310);	//  TCP	UDP	Outlaws (1997 video game). Both UDP and TCP are reserved); but only UDP is used
		oUsedPorts.insert(5310);	//  TCP	UDP	Ginever.net data communication port
		oUsedPorts.insert(5311);	//  TCP	UDP	Ginever.net data communication port
		oUsedPorts.insert(5312);	//  TCP	UDP	Ginever.net data communication port
		oUsedPorts.insert(5313);	//  TCP	UDP	Ginever.net data communication port
		oUsedPorts.insert(5314);	//  TCP	UDP	Ginever.net data communication port
		oUsedPorts.insert(5315);	//  TCP	UDP	Ginever.net data communication port
		oUsedPorts.insert(5349);	//  TCP		STUN); a protocol for NAT traversal (UDP is reserved)[48]
		oUsedPorts.insert(5349);	//  TCP		TURN); a protocol for NAT traversal (UDP is reserved)[49]
		oUsedPorts.insert(5351);	//  TCP	UDP	NAT Port Mapping Protocol—client-requested configuration for inbound connections through network address translators
		oUsedPorts.insert(5353);	//  	UDP	Multicast DNS (mDNS)
		oUsedPorts.insert(5355);	//  TCP	UDP	LLMNR—Link-Local Multicast Name Resolution); allows hosts to perform name resolution for hosts on the same local link (only provided by Windows Vista and Server 2008)
		oUsedPorts.insert(5357);	//  TCP	UDP	Web Services for Devices (WSDAPI) (only provided by Windows Vista); Windows 7 and Server 2008)
		oUsedPorts.insert(5358);	//  TCP	UDP	WSDAPI Applications to Use a Secure Channel (only provided by Windows Vista); Windows 7 and Server 2008)
		oUsedPorts.insert(5394);	//  	UDP	Kega Fusion); a Sega multi-console emulator
		oUsedPorts.insert(5402);	//  TCP	UDP	mftp); Stratacache OmniCast content delivery system MFTP file sharing protocol
		oUsedPorts.insert(5405);	//  TCP	UDP	NetSupport Manager
		oUsedPorts.insert(5412);	//  TCP	UDP	IBM Rational Synergy (Telelogic Synergy) (Continuus CM) Message Router
		oUsedPorts.insert(5421);	//  TCP	UDP	NetSupport Manager
		oUsedPorts.insert(5432);	//  TCP	UDP	PostgreSQL database system
		oUsedPorts.insert(5433);	//  TCP		Bouwsoft file/webserver[70]
		oUsedPorts.insert(5445);	//  	UDP	Cisco Unified Video Advantage
		oUsedPorts.insert(5450);	//  TCP		OSIsoft PI Server Client Access
		oUsedPorts.insert(5457);	//  TCP		OSIsoft PI Asset Framework Client Access
		oUsedPorts.insert(5458);	//  TCP		OSIsoft PI Notifications Client Access
		oUsedPorts.insert(5495);	//  TCP		Applix TM1 Admin server
		oUsedPorts.insert(5498);	//  TCP		Hotline tracker server connection
		oUsedPorts.insert(5499);	//  	UDP	Hotline tracker server discovery
		oUsedPorts.insert(5500);	//  TCP		VNC remote desktop protocol—for incoming listening viewer); Hotline control connection
		oUsedPorts.insert(5501);	//  TCP		Hotline file transfer connection
		oUsedPorts.insert(5517);	//  TCP		Setiqueue Proxy server client for SETI@Home project
		oUsedPorts.insert(5550);	//  TCP		Hewlett-Packard Data Protector
		oUsedPorts.insert(5555);	//  TCP		Freeciv versions up to 2.0); Hewlett-Packard Data Protector); McAfee EndPoint Encryption Database Server); SAP); Default for Microsoft Dynamics CRM 4.0
		oUsedPorts.insert(5556);	//  TCP	UDP	Freeciv
		oUsedPorts.insert(5591);	//  TCP		Default for Tidal Enterprise Scheduler master-Socket used for communication between Agent-to-Master); though can be changed
		oUsedPorts.insert(5631);	//  TCP		pcANYWHEREdata); Symantec pcAnywhere (version 7.52 and later[71])[72] data
		oUsedPorts.insert(5632);	//  	UDP	pcANYWHEREstat); Symantec pcAnywhere (version 7.52 and later) status
		oUsedPorts.insert(5656);	//  TCP		IBM Lotus Sametime p2p file transfer
		oUsedPorts.insert(5666);	//  TCP		NRPE (Nagios)
		oUsedPorts.insert(5667);	//  TCP		NSCA (Nagios)
		oUsedPorts.insert(5678);	//  	UDP	Mikrotik RouterOS Neighbor Discovery Protocol (MNDP)
		oUsedPorts.insert(5721);	//  TCP	UDP	Kaseya
		oUsedPorts.insert(5723);	//  TCP		Operations Manager[73]
		oUsedPorts.insert(5741);	//  TCP	UDP	IDA Discover Port 1
		oUsedPorts.insert(5742);	//  TCP	UDP	IDA Discover Port 2
		oUsedPorts.insert(5800);	//  TCP		VNC remote desktop protocol—for use over HTTP
		oUsedPorts.insert(5814);	//  TCP	UDP	Hewlett-Packard Support Automation (HP OpenView Self-Healing Services)
		oUsedPorts.insert(5850);	//  TCP		COMIT SE (PCR)
		oUsedPorts.insert(5852);	//  TCP		Adeona client: communications to OpenDHT
		oUsedPorts.insert(5900);	//  TCP	UDP	Virtual Network Computing (VNC) remote desktop protocol (used by Apple Remote Desktop and others)
		oUsedPorts.insert(5912);	//  TCP		Default for Tidal Enterprise Scheduler agent-Socket used for communication between Master-to-Agent); though can be changed
		oUsedPorts.insert(5938);	//  TCP	UDP	TeamViewer remote desktop protocol
		oUsedPorts.insert(5984);	//  TCP	UDP	CouchDB database server
		oUsedPorts.insert(5999);	//  TCP		CVSup file update tool[74]
		oUsedPorts.insert(6000);	//  TCP		X11—used between an X client and server over the network
		oUsedPorts.insert(6001);	//  	UDP	X11—used between an X client and server over the network
		oUsedPorts.insert(6005);	//  TCP		Default for BMC Software Control-M/Server—Socket used for communication between Control-M processes—though often changed during installation
		oUsedPorts.insert(6005);	//  TCP		Default for Camfrog chat & cam client
		oUsedPorts.insert(6050);	//  TCP		Arcserve backup
		oUsedPorts.insert(6050);	//  TCP		Nortel software
		oUsedPorts.insert(6051);	//  TCP		Arcserve backup
		oUsedPorts.insert(6072);	//  TCP		iOperator Protocol Signal Port
		oUsedPorts.insert(6086);	//  TCP		PDTP—FTP like file server in a P2P network
		oUsedPorts.insert(6100);	//  TCP		Vizrt System
		oUsedPorts.insert(6100);	//  TCP		Ventrilo This is the authentication port that must be allowed outbound for version 3 of Ventrilo
		oUsedPorts.insert(6101);	//  TCP		Backup Exec Agent Browser
		oUsedPorts.insert(6110);	//  TCP	UDP	softcm); HP Softbench CM
		oUsedPorts.insert(6111);	//  TCP	UDP	spc); HP Softbench Sub-Process Control
		oUsedPorts.insert(6112);	//  	UDP	"dtspcd"—a network daemon that accepts requests from clients to execute commands and launch applications remotely
		oUsedPorts.insert(6112);	//  TCP		"dtspcd"—a network daemon that accepts requests from clients to execute commands and launch applications remotely
		oUsedPorts.insert(6112);	//  TCP		Blizzard's Battle.net gaming service); ArenaNet gaming service); Relic gaming sercive
		oUsedPorts.insert(6112);	//  TCP		Club Penguin Disney online game for kids
		oUsedPorts.insert(6113);	//  TCP		Club Penguin Disney online game for kids
		oUsedPorts.insert(6129);	//  TCP		DameWare Remote Control
		oUsedPorts.insert(6257);	//  	UDP	WinMX (see also 6699)
		oUsedPorts.insert(6260);	//  TCP	UDP	planet M.U.L.E.
		oUsedPorts.insert(6262);	//  TCP		Sybase Advantage Database Server
		oUsedPorts.insert(6324);	//  TCP	UDP	Hall Research Device discovery and configuration
		oUsedPorts.insert(6343);	//  	UDP	SFlow); sFlow traffic monitoring
		oUsedPorts.insert(6346);	//  TCP	UDP	gnutella-svc); gnutella (FrostWire); Limewire); Shareaza); etc.)
		oUsedPorts.insert(6347);	//  TCP	UDP	gnutella-rtr); Gnutella alternate
		oUsedPorts.insert(6350);	//  TCP	UDP	App Discovery and Access Protocol
		oUsedPorts.insert(6389);	//  TCP		EMC CLARiiON
		oUsedPorts.insert(6432);	//  TCP		PgBouncer - A connection pooler for PostgreSQL
		oUsedPorts.insert(6444);	//  TCP	UDP	Sun Grid Engine—Qmaster Service
		oUsedPorts.insert(6445);	//  TCP	UDP	Sun Grid Engine—Execution Service
		oUsedPorts.insert(6502);	//  TCP	UDP	Netop Business Solutions - NetOp Remote Control
		oUsedPorts.insert(6503);	//  	UDP	Netop Business Solutions - NetOp School
		oUsedPorts.insert(6515);	//  TCP	UDP	Elipse RPC Protocol (REC)
		oUsedPorts.insert(6522);	//  TCP		Gobby (and other libobby-based software)
		oUsedPorts.insert(6523);	//  TCP		Gobby 0.5 (and other libinfinity-based software)
		oUsedPorts.insert(6543);	//  	UDP	Paradigm Research & Development Jetnet[75] default
		oUsedPorts.insert(6566);	//  TCP		SANE (Scanner Access Now Easy)—SANE network scanner daemon
		oUsedPorts.insert(6560);	//  TCP		Speech-Dispatcher daemon
		oUsedPorts.insert(6561);	//  TCP		Speech-Dispatcher daemon
		oUsedPorts.insert(6571);	//  		Windows Live FolderShare client
		oUsedPorts.insert(6600);	//  TCP		Music Playing Daemon (MPD)
		oUsedPorts.insert(6619);	//  TCP	UDP	odette-ftps); Odette File Transfer Protocol (OFTP) over TLS/SSL
		oUsedPorts.insert(6646);	//  	UDP	McAfee Network Agent
		oUsedPorts.insert(6660);	//  Start range: Internet Relay Chat (IRC)
		oUsedPorts.insert(6661);
		oUsedPorts.insert(6662);
		oUsedPorts.insert(6663);
		oUsedPorts.insert(6664);
		oUsedPorts.insert(6665);
		oUsedPorts.insert(6666);
		oUsedPorts.insert(6667);
		oUsedPorts.insert(6668);
		oUsedPorts.insert(6669);	//  TCP		Internet Relay Chat (IRC)
		oUsedPorts.insert(6679);	//  TCP	UDP	Osorno Automation Protocol (OSAUT)
		oUsedPorts.insert(6679);	//  TCP		IRC SSL (Secure Internet Relay Chat)—often used
		oUsedPorts.insert(6697);	//  TCP		IRC SSL (Secure Internet Relay Chat)—often used
		oUsedPorts.insert(6699);	//  TCP		WinMX (see also 6257)
		oUsedPorts.insert(6702);	//  TCP		Default for Tidal Enterprise Scheduler client-Socket used for communication between Client-to-Master); though can be changed
		oUsedPorts.insert(6771);	//  	UDP	Polycom server broadcast
		oUsedPorts.insert(6789);	//  TCP		Campbell Scientific Loggernet Software[76]
		oUsedPorts.insert(6881);
		oUsedPorts.insert(6882);
		oUsedPorts.insert(6883);
		oUsedPorts.insert(6884);
		oUsedPorts.insert(6885);
		oUsedPorts.insert(6886);
		oUsedPorts.insert(6887);	//  TCP	UDP	BitTorrent part of full range of ports used most often
		oUsedPorts.insert(6888);	//  TCP	UDP	MUSE
		oUsedPorts.insert(6888);	//  Start range: BitTorrent part of full range of ports used most often
		oUsedPorts.insert(6889);
		oUsedPorts.insert(6890);
		oUsedPorts.insert(6891);
		oUsedPorts.insert(6892);
		oUsedPorts.insert(6893);
		oUsedPorts.insert(6894);
		oUsedPorts.insert(6895);
		oUsedPorts.insert(6896);
		oUsedPorts.insert(6897);
		oUsedPorts.insert(6898);
		oUsedPorts.insert(6899);
		oUsedPorts.insert(6900);
		oUsedPorts.insert(6901);
		oUsedPorts.insert(6902);
		oUsedPorts.insert(6903);
		oUsedPorts.insert(6904);
		oUsedPorts.insert(6905);
		oUsedPorts.insert(6906);
		oUsedPorts.insert(6907);
		oUsedPorts.insert(6908);
		oUsedPorts.insert(6909);
		oUsedPorts.insert(6910);
		oUsedPorts.insert(6911);
		oUsedPorts.insert(6912);
		oUsedPorts.insert(6913);
		oUsedPorts.insert(6914);
		oUsedPorts.insert(6915);
		oUsedPorts.insert(6916);
		oUsedPorts.insert(6917);
		oUsedPorts.insert(6918);
		oUsedPorts.insert(6919);
		oUsedPorts.insert(6920);
		oUsedPorts.insert(6921);
		oUsedPorts.insert(6922);
		oUsedPorts.insert(6923);
		oUsedPorts.insert(6924);
		oUsedPorts.insert(6925);
		oUsedPorts.insert(6926);
		oUsedPorts.insert(6927);
		oUsedPorts.insert(6928);
		oUsedPorts.insert(6929);
		oUsedPorts.insert(6930);
		oUsedPorts.insert(6931);
		oUsedPorts.insert(6932);
		oUsedPorts.insert(6933);
		oUsedPorts.insert(6934);
		oUsedPorts.insert(6935);
		oUsedPorts.insert(6936);
		oUsedPorts.insert(6937);
		oUsedPorts.insert(6938);
		oUsedPorts.insert(6939);
		oUsedPorts.insert(6940);
		oUsedPorts.insert(6941);
		oUsedPorts.insert(6942);
		oUsedPorts.insert(6943);
		oUsedPorts.insert(6944);
		oUsedPorts.insert(6945);
		oUsedPorts.insert(6946);
		oUsedPorts.insert(6947);
		oUsedPorts.insert(6948);
		oUsedPorts.insert(6949);
		oUsedPorts.insert(6950);
		oUsedPorts.insert(6951);
		oUsedPorts.insert(6952);
		oUsedPorts.insert(6953);
		oUsedPorts.insert(6954);
		oUsedPorts.insert(6955);
		oUsedPorts.insert(6956);
		oUsedPorts.insert(6957);
		oUsedPorts.insert(6958);
		oUsedPorts.insert(6959);
		oUsedPorts.insert(6960);
		oUsedPorts.insert(6961);
		oUsedPorts.insert(6962);
		oUsedPorts.insert(6963);
		oUsedPorts.insert(6964);
		oUsedPorts.insert(6965);
		oUsedPorts.insert(6966);
		oUsedPorts.insert(6967);
		oUsedPorts.insert(6968);
		oUsedPorts.insert(6969);
		oUsedPorts.insert(6970);
		oUsedPorts.insert(6971);
		oUsedPorts.insert(6972);
		oUsedPorts.insert(6973);
		oUsedPorts.insert(6974);
		oUsedPorts.insert(6975);
		oUsedPorts.insert(6976);
		oUsedPorts.insert(6977);
		oUsedPorts.insert(6978);
		oUsedPorts.insert(6979);
		oUsedPorts.insert(6980);
		oUsedPorts.insert(6981);
		oUsedPorts.insert(6982);
		oUsedPorts.insert(6983);
		oUsedPorts.insert(6984);
		oUsedPorts.insert(6985);
		oUsedPorts.insert(6986);
		oUsedPorts.insert(6987);
		oUsedPorts.insert(6988);
		oUsedPorts.insert(6989);
		oUsedPorts.insert(6990);
		oUsedPorts.insert(6991);
		oUsedPorts.insert(6992);
		oUsedPorts.insert(6993);
		oUsedPorts.insert(6994);
		oUsedPorts.insert(6995);
		oUsedPorts.insert(6996);
		oUsedPorts.insert(6997);
		oUsedPorts.insert(6998);
		oUsedPorts.insert(6999);	//  TCP	UDP	BitTorrent part of full range of ports used most often
		oUsedPorts.insert(6891);	//  Start range: Windows Live Messenger (File transfer)
		oUsedPorts.insert(6892);
		oUsedPorts.insert(6893);
		oUsedPorts.insert(6894);
		oUsedPorts.insert(6895);
		oUsedPorts.insert(6896);
		oUsedPorts.insert(6897);
		oUsedPorts.insert(6898);
		oUsedPorts.insert(6899);
		oUsedPorts.insert(6900);	//  TCP	UDP	Windows Live Messenger (File transfer)
		oUsedPorts.insert(6901);	//  TCP	UDP	Windows Live Messenger (Voice)
		oUsedPorts.insert(6969);	//  TCP	UDP	acmsoda
		oUsedPorts.insert(6969);	//  TCP		BitTorrent tracker
		oUsedPorts.insert(7000);	//  TCP		Default for Vuze's built in HTTPS Bittorrent Tracker
		oUsedPorts.insert(7000);	//  TCP		Avira Server Management Console
		oUsedPorts.insert(7001);	//  TCP		Avira Server Management Console
		oUsedPorts.insert(7001);	//  TCP		Default for BEA WebLogic Server's HTTP server); though often changed during installation
		oUsedPorts.insert(7002);	//  TCP		Default for BEA WebLogic Server's HTTPS server); though often changed during installation
		oUsedPorts.insert(7005);	//  TCP		Default for BMC Software Control-M/Server and Control-M/Agent for Agent-to-Server); though often changed during installation
		oUsedPorts.insert(7006);	//  TCP		Default for BMC Software Control-M/Server and Control-M/Agent for Server-to-Agent); though often changed during installation
		oUsedPorts.insert(7010);	//  TCP		Default for Cisco AON AMC (AON Management Console) [77]
		oUsedPorts.insert(7022);	//  TCP		Database mirroring endpoints
		oUsedPorts.insert(7023);	//  	UDP	Bryan Wilcutt T2-NMCS Protocol for SatCom Modems
		oUsedPorts.insert(7025);	//  TCP		Zimbra LMTP [mailbox]—local mail delivery
		oUsedPorts.insert(7047);	//  TCP		Zimbra conversion server
		oUsedPorts.insert(7133);	//  TCP		Enemy Territory: Quake Wars
		oUsedPorts.insert(7144);	//  TCP		Peercast
		oUsedPorts.insert(7145);	//  TCP		Peercast
		oUsedPorts.insert(7171);	//  TCP		Tibia
		oUsedPorts.insert(7306);	//  TCP		Zimbra mysql [mailbox]
		oUsedPorts.insert(7307);	//  TCP		Zimbra mysql [logger]
		oUsedPorts.insert(7312);	//  	UDP	Sibelius License Server
		oUsedPorts.insert(7400);	//  TCP	UDP	RTPS (Real Time Publish Subscribe) DDS Discovery
		oUsedPorts.insert(7401);	//  TCP	UDP	RTPS (Real Time Publish Subscribe) DDS User-Traffic
		oUsedPorts.insert(7402);	//  TCP	UDP	RTPS (Real Time Publish Subscribe) DDS Meta-Traffic
		oUsedPorts.insert(7473);	//  TCP	UDP	Rise: The Vieneo Province
		oUsedPorts.insert(7547);	//  TCP	UDP	CPE WAN Management Protocol Technical Report 069
		oUsedPorts.insert(7615);	//  TCP		ISL Online[78] communication protocol
		oUsedPorts.insert(7624);	//  TCP	UDP	Instrument Neutral Distributed Interface
		oUsedPorts.insert(7652);	//  TCP		I2P anonymizing overlay network
		oUsedPorts.insert(7653);	//  TCP		I2P anonymizing overlay network
		oUsedPorts.insert(7654);	//  TCP		I2P anonymizing overlay network
		oUsedPorts.insert(7655);	//  	UDP	I2P SAM Bridge Socket API
		oUsedPorts.insert(7656);	//  TCP		I2P anonymizing overlay network
		oUsedPorts.insert(7657);	//  TCP		I2P anonymizing overlay network
		oUsedPorts.insert(7658);	//  TCP		I2P anonymizing overlay network
		oUsedPorts.insert(7659);	//  TCP		I2P anonymizing overlay network
		oUsedPorts.insert(7660);	//  TCP		I2P anonymizing overlay network
		oUsedPorts.insert(7670);	//  TCP		BrettspielWelt BSW Boardgame Portal
		oUsedPorts.insert(7676);	//  TCP		Aqumin AlphaVision Remote Command Interface
		oUsedPorts.insert(7700);	//  	UDP	P2P DC (RedHub)
		oUsedPorts.insert(7707);	//  	UDP	Killing Floor
		oUsedPorts.insert(7708);	//  	UDP	Killing Floor
		oUsedPorts.insert(7717);	//  	UDP	Killing Floor
		oUsedPorts.insert(7777);	//  TCP		iChat server file transfer proxy
		oUsedPorts.insert(7777);	//  TCP		Oracle Cluster File System 2
		oUsedPorts.insert(7777);	//  TCP		Windows backdoor program tini.exe default
		oUsedPorts.insert(7777);	//  TCP		Xivio default Chat Server
		oUsedPorts.insert(7777);	//  TCP		Terraria default server
		oUsedPorts.insert(7777);	//  	UDP	San Andreas Multiplayer default server
		oUsedPorts.insert(7778);	//  TCP		Bad Trip MUD
		oUsedPorts.insert(7777);	//  Start range: Unreal Tournament series default server
		oUsedPorts.insert(7778);
		oUsedPorts.insert(7779);
		oUsedPorts.insert(7780);
		oUsedPorts.insert(7781);
		oUsedPorts.insert(7782);
		oUsedPorts.insert(7783);
		oUsedPorts.insert(7784);
		oUsedPorts.insert(7785);
		oUsedPorts.insert(7786);
		oUsedPorts.insert(7787);
		oUsedPorts.insert(7788);	//  TCP	UDP	Unreal Tournament series default server
		oUsedPorts.insert(7787);	//  TCP		GFI EventsManager 7 & 8
		oUsedPorts.insert(7788);	//  TCP		GFI EventsManager 7 & 8
		oUsedPorts.insert(7831);	//  TCP		Default used by Smartlaunch Internet Cafe Administration[79] software
		oUsedPorts.insert(7880);	//  TCP	UDP	PowerSchool Gradebook Server
		oUsedPorts.insert(7890);	//  TCP		Default that will be used by the iControl Internet Cafe Suite Administration software
		oUsedPorts.insert(7915);	//  TCP		Default for YSFlight server[80]
		oUsedPorts.insert(7935);	//  TCP		Fixed port used for Adobe Flash Debug Player to communicate with a debugger (Flash IDE); Flex Builder or fdb).[81]
		oUsedPorts.insert(9936);	//  TCP	UDP	EMC2 (Legato) Networker or Sun Solcitice Backup
		oUsedPorts.insert(7937);	//  TCP	UDP	EMC2 (Legato) Networker or Sun Solcitice Backup
		oUsedPorts.insert(8000);	//  	UDP	iRDMI (Intel Remote Desktop Management Interface)[82]—sometimes erroneously used instead of port 8080
		oUsedPorts.insert(8000);	//  TCP		iRDMI (Intel Remote Desktop Management Interface)[82]—sometimes erroneously used instead of port 8080
		oUsedPorts.insert(8000);	//  TCP		Commonly used for internet radio streams such as those using SHOUTcast
		oUsedPorts.insert(8000);	//  TCP		FreemakeVideoCapture service a part of Freemake Video Downloader [83]
		oUsedPorts.insert(8001);	//  TCP		Commonly used for internet radio streams such as those using SHOUTcast
		oUsedPorts.insert(8002);	//  TCP		Cisco Systems Unified Call Manager Intercluster
		oUsedPorts.insert(8008);	//  TCP		HTTP Alternate
		oUsedPorts.insert(8008);	//  TCP		IBM HTTP Server administration default
		oUsedPorts.insert(8009);	//  TCP		ajp13—Apache JServ Protocol AJP Connector
		oUsedPorts.insert(8010);	//  TCP		XMPP File transfers
		oUsedPorts.insert(8011);	//  TCP		HTTP/TCP Symon Communications Event and Query Engine
		oUsedPorts.insert(8012);	//  TCP		HTTP/TCP Symon Communications Event and Query Engine
		oUsedPorts.insert(8013);	//  TCP		HTTP/TCP Symon Communications Event and Query Engine
		oUsedPorts.insert(8014);	//  TCP		HTTP/TCP Symon Communications Event and Query Engine
		oUsedPorts.insert(8014);	//  TCP	UDP	Perseus SDR Receiver default remote connection port
		oUsedPorts.insert(8020);	//  TCP		360Works SuperContainer[84]
		oUsedPorts.insert(8074);	//  TCP		Gadu-Gadu
		oUsedPorts.insert(8075);	//  TCP		Killing Floor
		oUsedPorts.insert(8078);	//  TCP	UDP	Default port for most Endless Online-based servers
		oUsedPorts.insert(8080);	//  TCP		HTTP alternate (http_alt)—commonly used for Web proxy and caching server); or for running a Web server as a non-root user
		oUsedPorts.insert(8080);	//  TCP		Apache Tomcat
		oUsedPorts.insert(8080);	//  	UDP	FilePhile Master/Relay
		oUsedPorts.insert(8081);	//  TCP		HTTP alternate); VibeStreamer); e.g. McAfee ePolicy Orchestrator (ePO)
		oUsedPorts.insert(8086);	//  TCP		HELM Web Host Automation Windows Control Panel
		oUsedPorts.insert(8086);	//  TCP		Kaspersky AV Control Center
		oUsedPorts.insert(8087);	//  TCP		Hosting Accelerator Control Panel
		oUsedPorts.insert(8087);	//  TCP		Parallels Plesk Control Panel
		oUsedPorts.insert(8087);	//  	UDP	Kaspersky AV Control Center
		oUsedPorts.insert(8088);	//  TCP		Asterisk (PBX) Web Configuration utility (GUI Addon)
		oUsedPorts.insert(8089);	//  TCP		Splunk Daemon
		oUsedPorts.insert(8090);	//  TCP		HTTP Alternate (http_alt_alt)—used as an alternative to port 8080
		oUsedPorts.insert(8100);	//  TCP		Console Gateway License Verification
		oUsedPorts.insert(8116);	//  	UDP	Check Point Cluster Control Protocol
		oUsedPorts.insert(8118);	//  TCP		Privoxy—advertisement-filtering Web proxy
		oUsedPorts.insert(8123);	//  TCP		Polipo Web proxy
		oUsedPorts.insert(8192);	//  TCP		Sophos Remote Management System
		oUsedPorts.insert(8193);	//  TCP		Sophos Remote Management System
		oUsedPorts.insert(8194);	//  TCP		Sophos Remote Management System
		oUsedPorts.insert(8194);	//  TCP		Bloomberg Application
		oUsedPorts.insert(8195);	//  TCP		Bloomberg Application
		oUsedPorts.insert(8200);	//  TCP		GoToMyPC
		oUsedPorts.insert(8222);	//  TCP		VMware Server Management User Interface[85] (insecure Web interface).[86] See also port 8333
		oUsedPorts.insert(8243);	//  TCP	UDP	HTTPS listener for Apache Synapse [87]
		oUsedPorts.insert(8280);	//  TCP	UDP	HTTP listener for Apache Synapse [87]
		oUsedPorts.insert(8291);	//  TCP		Winbox—Default on a MikroTik RouterOS for a Windows application used to administer MikroTik RouterOS
		oUsedPorts.insert(8303);	//  	UDP	Teeworlds Server
		oUsedPorts.insert(8331);	//  TCP		MultiBit); [5]
		oUsedPorts.insert(8332);	//  TCP		Bitcoin JSON-RPC server[88]
		oUsedPorts.insert(8333);	//  TCP		Bitcoin[89]
		oUsedPorts.insert(8333);	//  TCP		VMware Server Management User Interface[85] (secure Web interface).[86] See also port 8222
		oUsedPorts.insert(8400);	//  TCP	UDP	cvp); Commvault Unified Data Management
		oUsedPorts.insert(8442);	//  TCP	UDP	CyBro A-bus); Cybrotech Ltd.
		oUsedPorts.insert(8443);	//  TCP		SW Soft Plesk Control Panel); Apache Tomcat SSL); Promise WebPAM SSL); McAfee ePolicy Orchestrator (ePO)
		oUsedPorts.insert(8484);	//  TCP	UDP	MapleStory
		oUsedPorts.insert(8500);	//  TCP	UDP	ColdFusion Macromedia/Adobe ColdFusion default and Duke Nukem 3D—default
		oUsedPorts.insert(8501);	//  TCP		[6] DukesterX —default
		oUsedPorts.insert(8601);	//  TCP		Wavestore CCTV protocol[90]
		oUsedPorts.insert(8602);	//  TCP	UDP	Wavestore Notification protocol[90]
		oUsedPorts.insert(8642);	//  TCP		Lotus Traveller
		oUsedPorts.insert(8691);	//  TCP		Ultra Fractal default server port for distributing calculations over network computers
		oUsedPorts.insert(8701);	//  	UDP	SoftPerfect Bandwidth Manager
		oUsedPorts.insert(8702);	//  	UDP	SoftPerfect Bandwidth Manager
		oUsedPorts.insert(8767);	//  	UDP	TeamSpeak—default
		oUsedPorts.insert(8768);	//  	UDP	TeamSpeak—alternate
		oUsedPorts.insert(8778);	//  TCP		EPOS Speech Synthesis System
		oUsedPorts.insert(8840);	//  TCP		Opera Unite server
		oUsedPorts.insert(8880);	//  	UDP	cddbp-alt); CD DataBase (CDDB) protocol (CDDBP) alternate
		oUsedPorts.insert(8880);	//  TCP		cddbp-alt); CD DataBase (CDDB) protocol (CDDBP) alternate
		oUsedPorts.insert(8880);	//  TCP		WebSphere Application Server SOAP connector default
		oUsedPorts.insert(8880);	//  TCP		Win Media Streamer to Server SOAP connector default
		oUsedPorts.insert(8881);	//  TCP		Atlasz Informatics Research Ltd Secure Application Server
		oUsedPorts.insert(8882);	//  TCP		Atlasz Informatics Research Ltd Secure Application Server
		oUsedPorts.insert(8883);	//  TCP	UDP	Secure MQ Telemetry Transport (MQTT over SSL)
		oUsedPorts.insert(8886);	//  TCP		PPM3 (Padtec Management Protocol version 3)
		oUsedPorts.insert(8887);	//  TCP		HyperVM HTTP
		oUsedPorts.insert(8888);	//  TCP		HyperVM HTTPS
		oUsedPorts.insert(8888);	//  TCP		Freenet HTTP
		oUsedPorts.insert(8888);	//  TCP	UDP	NewsEDGE server
		oUsedPorts.insert(8888);	//  TCP		Sun Answerbook dwhttpd server (deprecated by docs.sun.com)
		oUsedPorts.insert(8888);	//  TCP		GNUmp3d HTTP music streaming and Web interface
		oUsedPorts.insert(8888);	//  TCP		LoLo Catcher HTTP Web interface (www.optiform.com)
		oUsedPorts.insert(8888);	//  TCP		D2GS Admin Console Telnet administration console for D2GS servers (Diablo 2)
		oUsedPorts.insert(8888);	//  TCP		Earthland Relams 2 Server (AU1_2)
		oUsedPorts.insert(8888);	//  TCP		MAMP Server
		oUsedPorts.insert(8889);	//  TCP		MAMP Server
		oUsedPorts.insert(8889);	//  TCP		Earthland Relams 2 Server (AU1_1)
		oUsedPorts.insert(8983);	//  TCP		Default for Apache Solr 1.4
		oUsedPorts.insert(8998);	//  TCP		I2P Monotone Repository
		oUsedPorts.insert(9000);	//  TCP		Buffalo LinkSystem Web access
		oUsedPorts.insert(9000);	//  TCP		DBGp
		oUsedPorts.insert(9000);	//  TCP		SqueezeCenter web server & streaming
		oUsedPorts.insert(9000);	//  	UDP	UDPCast
		oUsedPorts.insert(9000);	//  TCP		Play! Framework web server
		oUsedPorts.insert(9001);	//  TCP	UDP	ETL Service Manager[91]
		oUsedPorts.insert(9001);	//  		Microsoft SharePoint authoring environment
		oUsedPorts.insert(9001);	//  		cisco-xremote router configuration
		oUsedPorts.insert(9001);	//  		Tor network default
		oUsedPorts.insert(9001);	//  TCP		DBGp Proxy
		oUsedPorts.insert(9009);	//  TCP	UDP	Pichat Server—Peer to peer chat software
		oUsedPorts.insert(9010);	//  TCP		TISERVICEMANAGEMENT Numara Track-It!
		oUsedPorts.insert(9020);	//  TCP		WiT WiT Services
		oUsedPorts.insert(9025);	//  TCP		WiT WiT Services
		oUsedPorts.insert(9030);	//  TCP		Tor often used
		oUsedPorts.insert(9043);	//  TCP		WebSphere Application Server Administration Console secure
		oUsedPorts.insert(9050);	//  TCP		Tor
		oUsedPorts.insert(9051);	//  TCP		Tor
		oUsedPorts.insert(9060);	//  TCP		WebSphere Application Server Administration Console
		oUsedPorts.insert(9080);	//  	UDP	glrpc); Groove Collaboration software GLRPC
		oUsedPorts.insert(9080);	//  TCP		glrpc); Groove Collaboration software GLRPC
		oUsedPorts.insert(9080);	//  TCP		WebSphere Application Server HTTP Transport (port 1) default
		oUsedPorts.insert(9090);	//  TCP		Webwasher); Secure Web); McAfee Web Gateway - Default Proxy Port
		oUsedPorts.insert(9090);	//  TCP		Openfire Administration Console
		oUsedPorts.insert(9090);	//  TCP		SqueezeCenter control (CLI)
		oUsedPorts.insert(9091);	//  TCP		Openfire Administration Console (SSL Secured)
		oUsedPorts.insert(9091);	//  TCP		Transmission (BitTorrent client) Web Interface
		oUsedPorts.insert(9100);	//  TCP		PDL Data Stream
		oUsedPorts.insert(9101);	//  TCP	UDP	Bacula Director
		oUsedPorts.insert(9102);	//  TCP	UDP	Bacula File Daemon
		oUsedPorts.insert(9103);	//  TCP	UDP	Bacula Storage Daemon
		oUsedPorts.insert(9105);	//  TCP	UDP	Xadmin Control Daemon
		oUsedPorts.insert(9106);	//  TCP	UDP	Astergate Control Daemon
		oUsedPorts.insert(9107);	//  TCP		Astergate-FAX Control Daemon
		oUsedPorts.insert(9110);	//  	UDP	SSMP Message protocol
		oUsedPorts.insert(9119);	//  TCP	UDP	MXit Instant Messenger
		oUsedPorts.insert(9191);	//  TCP		Catamount Software - PocketMoney Sync
		oUsedPorts.insert(9293);	//  TCP		Sony PlayStation RemotePlay
		oUsedPorts.insert(9300);	//  TCP		IBM Cognos 8 SOAP Business Intelligence and Performance Management
		oUsedPorts.insert(9303);	//  	UDP	D-Link Shareport Share storage and MFP printers
		oUsedPorts.insert(9306);	//  TCP		Sphinx Native API
		oUsedPorts.insert(9312);	//  TCP		Sphinx SphinxQL
		oUsedPorts.insert(9418);	//  TCP	UDP	git); Git pack transfer service
		oUsedPorts.insert(9420);	//  TCP		MooseFS distributed file system—master server to chunk servers
		oUsedPorts.insert(9421);	//  TCP		MooseFS distributed file system—master server to clients
		oUsedPorts.insert(9422);	//  TCP		MooseFS distributed file system—chunk servers to clients
		oUsedPorts.insert(9535);	//  TCP	UDP	mngsuite); LANDesk Management Suite Remote Control
		oUsedPorts.insert(9536);	//  TCP	UDP	laes-bf); IP Fabrics Surveillance buffering function
		oUsedPorts.insert(9600);	//  	UDP	Omron FINS); OMRON FINS PLC communication
		oUsedPorts.insert(9675);	//  TCP	UDP	Spiceworks Desktop); IT Helpdesk Software
		oUsedPorts.insert(9676);	//  TCP	UDP	Spiceworks Desktop); IT Helpdesk Software
		oUsedPorts.insert(9695);	//  	UDP	CCNx
		oUsedPorts.insert(9800);	//  TCP	UDP	WebDAV Source
		oUsedPorts.insert(9800);	//  		WebCT e-learning portal
		oUsedPorts.insert(9875);	//  TCP		Club Penguin Disney online game for kids
		oUsedPorts.insert(9898);	//  	UDP	MonkeyCom
		oUsedPorts.insert(9898);	//  TCP		MonkeyCom
		oUsedPorts.insert(9898);	//  TCP		Tripwire—File Integrity Monitoring Software
		oUsedPorts.insert(9987);	//  	UDP	TeamSpeak 3 server default (voice) port (for the conflicting service see the IANA list)
		oUsedPorts.insert(9996);	//  TCP	UDP	Ryan's App "Ryan's App" Trading Software
		oUsedPorts.insert(9996);	//  TCP	UDP	The Palace "The Palace" Virtual Reality Chat software.—5
		oUsedPorts.insert(9998);	//  TCP	UDP	The Palace "The Palace" Virtual Reality Chat software.—5
		oUsedPorts.insert(9999);	//  		Hydranode—edonkey2000 TELNET control
		oUsedPorts.insert(9999);	//  TCP		Lantronix UDS-10/UDS100[92] RS-485 to Ethernet Converter TELNET control
		oUsedPorts.insert(9999);	//  		Urchin Web Analytics
		oUsedPorts.insert(10000);	//  		Webmin—Web-based Linux admin tool
		oUsedPorts.insert(10000);	//  		BackupExec
		oUsedPorts.insert(10000);	//  		Ericsson Account Manager (avim)
		oUsedPorts.insert(10001);	//  TCP		Lantronix UDS-10/UDS100[93] RS-485 to Ethernet Converter default
		oUsedPorts.insert(10008);	//  TCP	UDP	Octopus Multiplexer); primary port for the CROMP protocol); which provides a platform-independent means for communication of objects across a network
		oUsedPorts.insert(10009);	//  TCP	UDP	Cross Fire); a multiplayer online First Person Shooter
		oUsedPorts.insert(10010);	//  TCP		Open Object Rexx (ooRexx) rxapi daemon
		oUsedPorts.insert(10017);	//  		AIX);NeXT); HPUX—rexd daemon control
		oUsedPorts.insert(10024);	//  TCP		Zimbra smtp [mta]—to amavis from postfix
		oUsedPorts.insert(10025);	//  TCP		Zimbra smtp [mta]—back to postfix from amavis
		oUsedPorts.insert(10050);	//  TCP	UDP	Zabbix-Agent
		oUsedPorts.insert(10051);	//  TCP	UDP	Zabbix-Trapper
		oUsedPorts.insert(10110);	//  TCP	UDP	NMEA 0183 Navigational Data. Transport of NMEA 0183 sentences over TCP or UDP
		oUsedPorts.insert(10113);	//  TCP	UDP	NetIQ Endpoint
		oUsedPorts.insert(10114);	//  TCP	UDP	NetIQ Qcheck
		oUsedPorts.insert(10115);	//  TCP	UDP	NetIQ Endpoint
		oUsedPorts.insert(10116);	//  TCP	UDP	NetIQ VoIP Assessor
		oUsedPorts.insert(10172);	//  TCP		Intuit Quickbooks client
		oUsedPorts.insert(10200);	//  TCP		FRISK Software International's fpscand virus scanning daemon for Unix platforms [94]
		oUsedPorts.insert(10200);	//  TCP		FRISK Software International's f-protd virus scanning daemon for Unix platforms [95]
		oUsedPorts.insert(10201);	//  TCP		FRISK Software International's f-protd virus scanning daemon for Unix platforms [95]
		oUsedPorts.insert(10202);	//  TCP		FRISK Software International's f-protd virus scanning daemon for Unix platforms [95]
		oUsedPorts.insert(10203);	//  TCP		FRISK Software International's f-protd virus scanning daemon for Unix platforms [95]
		oUsedPorts.insert(10204);	//  TCP		FRISK Software International's f-protd virus scanning daemon for Unix platforms [95]
		oUsedPorts.insert(10301);	//  TCP		VoiceIP-ACS UMP default device provisioning endpoint
		oUsedPorts.insert(10302);	//  TCP		VoiceIP-ACS UMP default device provisioning endpoint (SSL)
		oUsedPorts.insert(10308);	//  		Lock-on: Modern Air Combat
		oUsedPorts.insert(10480);	//  		SWAT 4 Dedicated Server
		oUsedPorts.insert(10823);	//  	UDP	Farming Simulator 2011 Default Server
		oUsedPorts.insert(10891);	//  TCP		Jungle Disk (this port is opened by the Jungle Disk Monitor service on the localhost)
		oUsedPorts.insert(11001);	//  TCP	UDP	metasys ( Johnson Controls Metasys java AC control environment )
		oUsedPorts.insert(11211);	//  TCP	UDP	memcached
		oUsedPorts.insert(11235);	//  		Savage:Battle for Newerth Server Hosting
		oUsedPorts.insert(11294);	//  		Blood Quest Online Server
		oUsedPorts.insert(11371);	//  		OpenPGP HTTP key server
		oUsedPorts.insert(11576);	//  		IPStor Server management communication
		oUsedPorts.insert(12010);	//  TCP		ElevateDB default database port [96]
		oUsedPorts.insert(12011);	//  TCP		Axence nVision
		oUsedPorts.insert(12012);	//  TCP		Axence nVision
		oUsedPorts.insert(12012);	//  TCP		Audition Online Dance Battle); Korea Server—Status/Version Check
		oUsedPorts.insert(12012);	//  	UDP	Audition Online Dance Battle); Korea Server—Status/Version Check
		oUsedPorts.insert(12013);	//  TCP	UDP	Audition Online Dance Battle); Korea Server
		oUsedPorts.insert(12035);	//  	UDP	Linden Lab viewer to sim on SecondLife
		oUsedPorts.insert(12222);	//  	UDP	Light Weight Access Point Protocol (LWAPP) LWAPP data (RFC 5412)
		oUsedPorts.insert(12223);	//  	UDP	Light Weight Access Point Protocol (LWAPP) LWAPP control (RFC 5412)
		oUsedPorts.insert(12345);	//  		NetBus—remote administration tool (often Trojan horse). Also used by NetBuster. Little Fighter 2 (TCP).
		oUsedPorts.insert(12489);	//  TCP		NSClient/NSClient++/NC_Net (Nagios)
		oUsedPorts.insert(12975);	//  TCP		LogMeIn Hamachi (VPN tunnel software; also port 32976)—used to connect to Mediation Server (bibi.hamachi.cc); will attempt to use SSL (TCP port 443) if both 12975 & 32976 fail to connect
		oUsedPorts.insert(12998);	//  	UDP	Takenaka RDI Mirror World on SecondLife
		oUsedPorts.insert(12999);	//  	UDP	Takenaka RDI Mirror World on SecondLife
		oUsedPorts.insert(13000);	//  Start range: Linden Lab viewer to sim on SecondLife
		oUsedPorts.insert(13001);
		oUsedPorts.insert(13002);
		oUsedPorts.insert(13003);
		oUsedPorts.insert(13004);
		oUsedPorts.insert(13005);
		oUsedPorts.insert(13006);
		oUsedPorts.insert(13007);
		oUsedPorts.insert(13008);
		oUsedPorts.insert(13009);
		oUsedPorts.insert(13010);
		oUsedPorts.insert(13011);
		oUsedPorts.insert(13012);
		oUsedPorts.insert(13013);
		oUsedPorts.insert(13014);
		oUsedPorts.insert(13015);
		oUsedPorts.insert(13016);
		oUsedPorts.insert(13017);
		oUsedPorts.insert(13018);
		oUsedPorts.insert(13019);
		oUsedPorts.insert(13020);
		oUsedPorts.insert(13021);
		oUsedPorts.insert(13022);
		oUsedPorts.insert(13023);
		oUsedPorts.insert(13024);
		oUsedPorts.insert(13025);
		oUsedPorts.insert(13026);
		oUsedPorts.insert(13027);
		oUsedPorts.insert(13028);
		oUsedPorts.insert(13029);
		oUsedPorts.insert(13030);
		oUsedPorts.insert(13031);
		oUsedPorts.insert(13032);
		oUsedPorts.insert(13033);
		oUsedPorts.insert(13034);
		oUsedPorts.insert(13035);
		oUsedPorts.insert(13036);
		oUsedPorts.insert(13037);
		oUsedPorts.insert(13038);
		oUsedPorts.insert(13039);
		oUsedPorts.insert(13040);
		oUsedPorts.insert(13041);
		oUsedPorts.insert(13042);
		oUsedPorts.insert(13043);
		oUsedPorts.insert(13044);
		oUsedPorts.insert(13045);
		oUsedPorts.insert(13046);
		oUsedPorts.insert(13047);
		oUsedPorts.insert(13048);
		oUsedPorts.insert(13049);
		oUsedPorts.insert(13050);	//  	UDP	Linden Lab viewer to sim on SecondLife
		oUsedPorts.insert(13008);	//  TCP	UDP	Cross Fire); a multiplayer online First Person Shooter
		oUsedPorts.insert(13075);	//  TCP		Default[97] for BMC Software Control-M/Enterprise Manager Corba communication); though often changed during installation
		oUsedPorts.insert(13195);	//  TCP	UDP	Ontolux Ontolux 2D
		oUsedPorts.insert(13196);	//  TCP	UDP	Ontolux Ontolux 2D
		oUsedPorts.insert(13337);	//  TCP	UDP	ÆtherNet peer-to-peer networking
		oUsedPorts.insert(13338);	//  TCP	UDP	ÆtherNet peer-to-peer networking
		oUsedPorts.insert(13339);	//  TCP	UDP	ÆtherNet peer-to-peer networking
		oUsedPorts.insert(13340);	//  TCP	UDP	ÆtherNet peer-to-peer networking
		oUsedPorts.insert(13720);	//  TCP	UDP	Symantec NetBackup—bprd (formerly VERITAS)
		oUsedPorts.insert(13721);	//  TCP	UDP	Symantec NetBackup—bpdbm (formerly VERITAS)
		oUsedPorts.insert(13724);	//  TCP	UDP	Symantec Network Utility—vnetd (formerly VERITAS)
		oUsedPorts.insert(13782);	//  TCP	UDP	Symantec NetBackup—bpcd (formerly VERITAS)
		oUsedPorts.insert(13783);	//  TCP	UDP	Symantec VOPIED protocol (formerly VERITAS)
		oUsedPorts.insert(13785);	//  TCP	UDP	Symantec NetBackup Database—nbdb (formerly VERITAS)
		oUsedPorts.insert(13786);	//  TCP	UDP	Symantec nomdb (formerly VERITAS)
		oUsedPorts.insert(14439);	//  TCP		APRS UI-View Amateur Radio[98] UI-WebServer
		oUsedPorts.insert(14567);	//  	UDP	Battlefield 1942 and mods
		oUsedPorts.insert(14900);	//  TCP		K3 SYSPRO K3 Framework WCF Backbone
		oUsedPorts.insert(15000);	//  TCP		psyBNC
		oUsedPorts.insert(15000);	//  TCP		Wesnoth
		oUsedPorts.insert(15000);	//  TCP		Kaspersky Network Agent
		oUsedPorts.insert(15000);	//  TCP		hydap); Hypack Hydrographic Software Packages Data Acquisition
		oUsedPorts.insert(15000);	//  	UDP	hydap); Hypack Hydrographic Software Packages Data Acquisition
		oUsedPorts.insert(15556);	//  TCP	UDP	Jeex.EU Artesia (direct client-to-db.service)
		oUsedPorts.insert(15567);	//  	UDP	Battlefield Vietnam and mods
		oUsedPorts.insert(15345);	//  TCP	UDP	XPilot Contact
		oUsedPorts.insert(16000);	//  TCP		shroudBNC
		oUsedPorts.insert(16080);	//  TCP		Mac OS X Server Web (HTTP) service with performance cache[99]
		oUsedPorts.insert(16200);	//  TCP		Oracle Universal Content Management Content Server
		oUsedPorts.insert(16250);	//  TCP		Oracle Universal Content Management Inbound Refinery
		oUsedPorts.insert(16384);	//  	UDP	Iron Mountain Digital online backup
		oUsedPorts.insert(16567);	//  	UDP	Battlefield 2 and mods
		oUsedPorts.insert(17500);	//  TCP		Dropbox LanSync Protocol (db-lsp); used to synchronize file catalogs between Dropbox clients on your local network.
		oUsedPorts.insert(17500);	//  	UDP	Dropbox LanSync Discovery (db-lsp-disc); used to synchronize file catalogs between Dropbox clients on your local network; is transmitted to broadcast addresses.
		oUsedPorts.insert(18010);	//  TCP		Super Dancer Online Extreme(SDO-X)—CiB Net Station Malaysia Server
		oUsedPorts.insert(18104);	//  TCP		RAD PDF Service
		oUsedPorts.insert(18180);	//  TCP		DART Reporting server
		oUsedPorts.insert(18200);	//  TCP	UDP	Audition Online Dance Battle); AsiaSoft Thailand Server—Status/Version Check
		oUsedPorts.insert(18201);	//  TCP	UDP	Audition Online Dance Battle); AsiaSoft Thailand Server
		oUsedPorts.insert(18206);	//  TCP	UDP	Audition Online Dance Battle); AsiaSoft Thailand Server—FAM Database
		oUsedPorts.insert(18300);	//  TCP	UDP	Audition Online Dance Battle); AsiaSoft SEA Server—Status/Version Check
		oUsedPorts.insert(18301);	//  TCP	UDP	Audition Online Dance Battle); AsiaSoft SEA Server
		oUsedPorts.insert(18306);	//  TCP	UDP	Audition Online Dance Battle); AsiaSoft SEA Server—FAM Database
		oUsedPorts.insert(18333);	//  TCP		Bitcoin testnet[89]
		oUsedPorts.insert(18400);	//  TCP	UDP	Audition Online Dance Battle); KAIZEN Brazil Server—Status/Version Check
		oUsedPorts.insert(18401);	//  TCP	UDP	Audition Online Dance Battle); KAIZEN Brazil Server
		oUsedPorts.insert(18505);	//  TCP	UDP	Audition Online Dance Battle); Nexon Server—Status/Version Check
		oUsedPorts.insert(18506);	//  TCP	UDP	Audition Online Dance Battle); Nexon Server
		oUsedPorts.insert(18605);	//  TCP	UDP	X-BEAT—Status/Version Check
		oUsedPorts.insert(18606);	//  TCP	UDP	X-BEAT
		oUsedPorts.insert(19000);	//  TCP	UDP	Audition Online Dance Battle); G10/alaplaya Server—Status/Version Check
		oUsedPorts.insert(19000);	//  	UDP	JACK sound server
		oUsedPorts.insert(19001);	//  TCP	UDP	Audition Online Dance Battle); G10/alaplaya Server
		oUsedPorts.insert(19226);	//  TCP		Panda Software AdminSecure Communication Agent
		oUsedPorts.insert(19283);	//  TCP	UDP	K2 - KeyAuditor & KeyServer); Sassafras Software Inc. Software Asset Management tools
		oUsedPorts.insert(19294);	//  TCP		Google Talk Voice and Video connections [100]
		oUsedPorts.insert(19295);	//  	UDP	Google Talk Voice and Video connections [100]
		oUsedPorts.insert(19302);	//  	UDP	Google Talk Voice and Video connections [100]
		oUsedPorts.insert(19315);	//  TCP	UDP	KeyShadow for K2 - KeyAuditor & KeyServer); Sassafras Software Inc. Software Asset Management tools
		oUsedPorts.insert(19540);	//  TCP	UDP	Belkin Network USB Hub
		oUsedPorts.insert(19638);	//  TCP		Ensim Control Panel
		oUsedPorts.insert(19812);	//  TCP		4D database SQL Communication
		oUsedPorts.insert(19813);	//  TCP		4D database Client Server Communication
		oUsedPorts.insert(19814);	//  TCP		4D database DB4D Communication
		oUsedPorts.insert(19999);	//  		DNP - Secure (Distributed Network Protocol - Secure)); a secure version of the protocol used in SCADA systems between communicating RTU's and IED's
		oUsedPorts.insert(20000);	//  		DNP (Distributed Network Protocol)); a protocol used in SCADA systems between communicating RTU's and IED's
		oUsedPorts.insert(20000);	//  		Usermin); Web-based user tool
		oUsedPorts.insert(20014);	//  TCP		DART Reporting server
		oUsedPorts.insert(20560);	//  TCP	UDP	Killing Floor
		oUsedPorts.insert(20702);	//  TCP		Precise TPM Listener Agent
		oUsedPorts.insert(20720);	//  TCP		Symantec i3 Web GUI server
		oUsedPorts.insert(20790);	//  TCP		Precise TPM Web GUI server
		oUsedPorts.insert(21001);	//  TCP		AMLFilter); AMLFilter Inc. amlf-admin default port
		oUsedPorts.insert(21011);	//  TCP		AMLFilter); AMLFilter Inc. amlf-engine-01 default http port
		oUsedPorts.insert(21012);	//  TCP		AMLFilter); AMLFilter Inc. amlf-engine-01 default https port
		oUsedPorts.insert(21021);	//  TCP		AMLFilter); AMLFilter Inc. amlf-engine-02 default http port
		oUsedPorts.insert(21022);	//  TCP		AMLFilter); AMLFilter Inc. amlf-engine-02 default https port
		oUsedPorts.insert(22136);	//  TCP		FLIR Systems Camera Resource Protocol
		oUsedPorts.insert(22222);	//  TCP		Davis Instruments); WeatherLink IP
		oUsedPorts.insert(22347);	//  TCP	UDP	WibuKey); WIBU-SYSTEMS AG Software protection system
		oUsedPorts.insert(22349);	//  TCP		Wolfson Microelectronics WISCEBridge Debug Protocol[101]
		oUsedPorts.insert(22350);	//  TCP	UDP	CodeMeter); WIBU-SYSTEMS AG Software protection system
		oUsedPorts.insert(23073);	//  		Soldat Dedicated Server
		oUsedPorts.insert(23399);	//  		Skype Default Protocol
		oUsedPorts.insert(23513);	//  		Duke Nukem 3D#Source code Duke Nukem Ports
		oUsedPorts.insert(24444);	//  		NetBeans integrated development environment
		oUsedPorts.insert(24465);	//  TCP	UDP	Tonido Directory Server for Tonido which is a Personal Web App and P2P platform
		oUsedPorts.insert(24554);	//  TCP	UDP	BINKP); Fidonet mail transfers over TCP/IP
		oUsedPorts.insert(24800);	//  		Synergy: keyboard/mouse sharing software
		oUsedPorts.insert(24842);	//  		StepMania: Online: Dance Dance Revolution Simulator
		oUsedPorts.insert(25000);	//  TCP		Teamware Office standard client connection
		oUsedPorts.insert(25003);	//  TCP		Teamware Office client notifier
		oUsedPorts.insert(25005);	//  TCP		Teamware Office message transfer
		oUsedPorts.insert(25007);	//  TCP		Teamware Office MIME Connector
		oUsedPorts.insert(25010);	//  TCP		Teamware Office Agent server
		oUsedPorts.insert(25560);	//  TCP		codeheart.js Relay Server
		oUsedPorts.insert(25565);	//  TCP	UDP	Minecraft Dedicated Server
		oUsedPorts.insert(25565);	//  		MySQL Standard MySQL port
		oUsedPorts.insert(25570);	//  		Manic Digger default single player port
		oUsedPorts.insert(25826);	//  	UDP	collectd default port[102]
		oUsedPorts.insert(25888);	//  	UDP	Xfire (Firewall Report); UDP_IN) IP Address (206.220.40.146) resolves to gameservertracking.xfire.com. Use unknown.
		oUsedPorts.insert(25999);	//  TCP		Xfire
		oUsedPorts.insert(26000);	//  	UDP	id Software's Quake server
		oUsedPorts.insert(26000);	//  TCP		id Software's Quake server
		oUsedPorts.insert(26000);	//  TCP		CCP's EVE Online Online gaming MMORPG
		oUsedPorts.insert(26850);	//  TCP		War of No Return Server Port
		oUsedPorts.insert(26900);	//  TCP		CCP's EVE Online Online gaming MMORPG
		oUsedPorts.insert(26901);	//  TCP		CCP's EVE Online Online gaming MMORPG
		oUsedPorts.insert(27000);	//  Start range: Steam Client
		oUsedPorts.insert(27001);
		oUsedPorts.insert(27002);
		oUsedPorts.insert(27003);
		oUsedPorts.insert(27004);
		oUsedPorts.insert(27005);
		oUsedPorts.insert(27006);
		oUsedPorts.insert(27007);
		oUsedPorts.insert(27008);
		oUsedPorts.insert(27009);
		oUsedPorts.insert(27010);
		oUsedPorts.insert(27011);
		oUsedPorts.insert(27012);
		oUsedPorts.insert(27013);
		oUsedPorts.insert(27014);
		oUsedPorts.insert(27015);
		oUsedPorts.insert(27016);
		oUsedPorts.insert(27017);
		oUsedPorts.insert(27018);
		oUsedPorts.insert(27019);
		oUsedPorts.insert(27020);
		oUsedPorts.insert(27021);
		oUsedPorts.insert(27022);
		oUsedPorts.insert(27023);
		oUsedPorts.insert(27024);
		oUsedPorts.insert(27025);
		oUsedPorts.insert(27026);
		oUsedPorts.insert(27027);
		oUsedPorts.insert(27028);
		oUsedPorts.insert(27029);
		oUsedPorts.insert(27030);	//  	UDP	Steam Client
		oUsedPorts.insert(27000);	//  	UDP	(through 27006) id Software's QuakeWorld master server
		oUsedPorts.insert(27000);	//  Start range: FlexNet Publisher's License server (from the range of default ports)
		oUsedPorts.insert(27001);
		oUsedPorts.insert(27002);
		oUsedPorts.insert(27003);
		oUsedPorts.insert(27004);
		oUsedPorts.insert(27005);
		oUsedPorts.insert(27006);
		oUsedPorts.insert(27007);
		oUsedPorts.insert(27008);
		oUsedPorts.insert(27009);	//  TCP		FlexNet Publisher's License server (from the range of default ports)
		oUsedPorts.insert(27010);	//  		Source engine dedicated server port
		oUsedPorts.insert(27014);	//  Start range: Steam Downloads
		oUsedPorts.insert(27015);
		oUsedPorts.insert(27016);
		oUsedPorts.insert(27017);
		oUsedPorts.insert(27018);
		oUsedPorts.insert(27019);
		oUsedPorts.insert(27020);
		oUsedPorts.insert(27021);
		oUsedPorts.insert(27022);
		oUsedPorts.insert(27023);
		oUsedPorts.insert(27024);
		oUsedPorts.insert(27025);
		oUsedPorts.insert(27026);
		oUsedPorts.insert(27027);
		oUsedPorts.insert(27028);
		oUsedPorts.insert(27029);
		oUsedPorts.insert(27030);
		oUsedPorts.insert(27031);
		oUsedPorts.insert(27032);
		oUsedPorts.insert(27033);
		oUsedPorts.insert(27034);
		oUsedPorts.insert(27035);
		oUsedPorts.insert(27036);
		oUsedPorts.insert(27037);
		oUsedPorts.insert(27038);
		oUsedPorts.insert(27039);
		oUsedPorts.insert(27040);
		oUsedPorts.insert(27041);
		oUsedPorts.insert(27042);
		oUsedPorts.insert(27043);
		oUsedPorts.insert(27044);
		oUsedPorts.insert(27045);
		oUsedPorts.insert(27046);
		oUsedPorts.insert(27047);
		oUsedPorts.insert(27048);
		oUsedPorts.insert(27049);
		oUsedPorts.insert(27050);	//  TCP		Steam Downloads
		oUsedPorts.insert(27014);	//  		Source engine dedicated server port (rare)
		oUsedPorts.insert(27015);	//  		GoldSrc and Source engine dedicated server port
		oUsedPorts.insert(27016);	//  		Magicka server port
		oUsedPorts.insert(27017);	//  		mongoDB server port
		oUsedPorts.insert(27374);	//  		Sub7 default.
		oUsedPorts.insert(27888);	//  	UDP	Kaillera server
		oUsedPorts.insert(27900);	//  		Nintendo Wi-Fi Connection
		oUsedPorts.insert(27901);	//  		Nintendo Wi-Fi Connection
		oUsedPorts.insert(27902);	//  Start range: id Software's Quake II master server
		oUsedPorts.insert(27903);
		oUsedPorts.insert(27904);
		oUsedPorts.insert(27905);
		oUsedPorts.insert(27906);
		oUsedPorts.insert(27907);
		oUsedPorts.insert(27908);
		oUsedPorts.insert(27909);
		oUsedPorts.insert(27910);	//  	UDP	id Software's Quake II master server
		oUsedPorts.insert(27950);	//  	UDP	OpenArena outgoing
		oUsedPorts.insert(27960);	//  Start range: Activision's Enemy Territory and id Software's Quake III Arena); Quake III and Quake Live and some ioquake3 derived games (OpenArena incoming)
		oUsedPorts.insert(27961);
		oUsedPorts.insert(27962);
		oUsedPorts.insert(27963);
		oUsedPorts.insert(27964);
		oUsedPorts.insert(27965);
		oUsedPorts.insert(27966);
		oUsedPorts.insert(27967);
		oUsedPorts.insert(27968);
		oUsedPorts.insert(27969);	//  	UDP	Activision's Enemy Territory and id Software's Quake III Arena); Quake III and Quake Live and some ioquake3 derived games (OpenArena incoming)
		oUsedPorts.insert(28000);	//  		Bitfighter Common/default Bitfighter Server
		oUsedPorts.insert(28001);	//  		Starsiege: Tribes Common/default Tribes v.1 Server
		oUsedPorts.insert(28395);	//  TCP		www.SmartSystemsLLC.com Used by Smart Sale 5.0
		oUsedPorts.insert(28785);	//  	UDP	Cube 2 Sauerbraten[103]
		oUsedPorts.insert(28786);	//  	UDP	Cube 2 Sauerbraten Port 2[103]
		oUsedPorts.insert(28852);	//  TCP	UDP	Killing Floor
		oUsedPorts.insert(28910);	//  		Nintendo Wi-Fi Connection
		oUsedPorts.insert(28960);	//  	UDP	Call of Duty; Call of Duty: United Offensive; Call of Duty 2; Call of Duty 4: Modern Warfare; Call of Duty: World at War (PC Version)
		oUsedPorts.insert(29000);	//  		Perfect World International Used by the Perfect World International Client
		oUsedPorts.insert(29292);	//  TCP		TMO Integration Service Communications Port); Used by Transaction Manager SaaS (HighJump Software)
		oUsedPorts.insert(29900);	//  		Nintendo Wi-Fi Connection
		oUsedPorts.insert(29901);	//  		Nintendo Wi-Fi Connection
		oUsedPorts.insert(29920);	//  		Nintendo Wi-Fi Connection
		oUsedPorts.insert(30000);	//  		Pokémon Netbattle
		oUsedPorts.insert(30301);	//  		BitTorrent
		oUsedPorts.insert(30564);	//  TCP		Multiplicity: keyboard/mouse/clipboard sharing software
		oUsedPorts.insert(30718);	//  	UDP	Lantronix Discovery for Lantronix serial-to-ethernet devices
		oUsedPorts.insert(30777);	//  TCP		ZangZing agent
		oUsedPorts.insert(31337);	//  TCP		Back Orifice—remote administration tool (often Trojan horse)
		oUsedPorts.insert(31415);	//  		ThoughtSignal—Server Communication Service (often Informational)
		oUsedPorts.insert(31456);	//  TCP		TetriNET IRC gateway on some servers
		oUsedPorts.insert(31457);	//  TCP		TetriNET
		oUsedPorts.insert(31458);	//  TCP		TetriNET Used for game spectators
		oUsedPorts.insert(31620);	//  TCP	UDP	LM-MON (Standard Floating License Manager LM-MON)
		oUsedPorts.insert(32123);	//  TCP		x3Lobby Used by x3Lobby); an internet application.
		oUsedPorts.insert(32245);	//  TCP		MMTSG-mutualed over MMT (encrypted transmission)
		oUsedPorts.insert(32769);	//  TCP		FileNet RPC
		oUsedPorts.insert(32887);	//  TCP		Use by "Ace of Spades" game
		oUsedPorts.insert(32976);	//  TCP		LogMeIn Hamachi (VPN tunnel software; also port 12975)—used to connect to Mediation Server (bibi.hamachi.cc); will attempt to use SSL (TCP port 443) if both 12975 & 32976 fail to connect
		oUsedPorts.insert(33434);	//  TCP	UDP	traceroute
		oUsedPorts.insert(33982);	//  TCP	UDP	Dezta software
		oUsedPorts.insert(34443);	//  		Linksys PSUS4 print server
		oUsedPorts.insert(34567);	//  TCP		EDI service[104]
		oUsedPorts.insert(36963);	//  	UDP	Any of the USGN online games); most notably Counter Strike 2D multiplayer (2D clone of popular CounterStrike computer game)
		oUsedPorts.insert(37659);	//  TCP		Axence nVision
		oUsedPorts.insert(37777);	//  TCP		Digital Video Recorder hardware
		oUsedPorts.insert(40000);	//  TCP	UDP	SafetyNET p Real-time Industrial Ethernet protocol
		oUsedPorts.insert(40123);	//  	UDP	Flatcast
		oUsedPorts.insert(41823);	//  TCP	UDP	Murealm Client
		oUsedPorts.insert(43047);	//  TCP		TheòsMessenger second port for service TheòsMessenger
		oUsedPorts.insert(43034);	//  TCP	UDP	LarmX.com™ database update mtr port
		oUsedPorts.insert(43048);	//  TCP		TheòsMessenger third port for service TheòsMessenger
		oUsedPorts.insert(43594);	//  TCP		RuneScape); FunOrb); Runescape Private Servers game servers
		oUsedPorts.insert(43595);	//  TCP		RuneScape JAGGRAB servers
		oUsedPorts.insert(45824);	//  TCP		Server for the DAI family of client-server products
		oUsedPorts.insert(47001);	//  TCP		WinRM - Windows Remote Management Service [105]
		oUsedPorts.insert(47808);	//  TCP	UDP	BACnet Building Automation and Control Networks (4780810 = BAC016)
		oUsedPorts.insert(49151);	//  TCP	UDP	Reserved


		for ( short i = 27500; i <= 27900; ++i )
			oUsedPorts.insert( i ); //27500-27900);	//  	UDP	id Software's QuakeWorld
	}

	short nPort = 1024;

	while ( oUsedPorts.count( nPort ) )
	{
		nPort = common::getRandomNum( 1024, 49151 );
	}

	return nPort;
}

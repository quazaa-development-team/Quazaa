#ifndef SECURITY_H
#define SECURITY_H

#include <list>
#include <map>
#include <queue>
#include <set>

#include <QHostAddress>
#include <QMutex>
#include <QString>
#include <QUuid>

#include "securerule.h"
#include "time.h"

/*class CSecureRule;
class CIPRule;
class CIPRangeRule;
class CCountryRule;
class CHashRule;
class CRegExpRule;
class CUserAgentRule;
class CContentRule;*/

// Increment this if there have been made changes to the way of storing security rules.
#define SECURITY_CODE_VERSION	1
// History:
// 1 - Initial implementation

namespace security
{
	typedef enum { tri_unknown, tri_true, tri_false } tristate;

	class CSecurity
	{
		/* ================================================================ */
		/* ========================== Attributes ========================== */
		/* ================================================================ */
	public:
		QMutex					m_pSection;
		static const QString	xmlns;

		typedef enum
		{
			banSession, ban5Mins, ban30Mins, ban2Hours, banWeek, banMonth, banForever
		} BanLength;

	private:
		typedef std::pair< QString, CHashRule* > CHashPair;

		typedef std::list< CSecureRule*  > CSecurityRuleList;
		typedef std::list< CIPRangeRule* > CIPRangeRuleList;
		typedef std::list< CRegExpRule*  > CRegExpRuleList;
		typedef std::list< CContentRule* > CContentRuleList;

		typedef std::queue< CSecureRule* > CNewRulesQueue;

		typedef std::set< QString > CMissCache;

		typedef std::map< QString, CIPRule*        > CAddressRuleMap;
		typedef std::map< QString, CCountryRule*   > CCountryRuleMap;
		typedef std::map< QString, CUserAgentRule* > CUserAgentRuleMap;

		// Note: Using a multimap eliminates eventual problems of hash
		// collisions caused by weaker hashes like MD5 for example.
		typedef std::multimap< QString, CHashRule* > CHashRuleMap;

		typedef CSecurityRuleList::const_iterator CIterator;

		// contains all rules
		CSecurityRuleList	m_Rules;

		// Used to manage newly added rules during sanity check
		CSecurityRuleList	m_loadedAddressRules;
		CNewRulesQueue		m_newAddressRules;
		CSecurityRuleList	m_loadedHitRules;
		CNewRulesQueue		m_newHitRules;

		// IP rule miss cache
		CMissCache			m_Cache;

		// single IP blocking rules
		CAddressRuleMap		m_IPs;

		// multiple IP blocking rules
		CIPRangeRuleList	m_IPRanges;

		// country rules
		CCountryRuleMap		m_Countries;

		// hash rules
		CHashRuleMap		m_Hashes;

		// all other content rules
		CContentRuleList	m_Contents;

		// RegExp rules
		CRegExpRuleList		m_RegExpressions;

		// User agent rules
		CUserAgentRuleMap	m_UserAgents;

		// Expire Counters
		quint32				m_nLastRuleExpiryCheck;
		quint32				m_nRuleExpiryInterval;
		quint32				m_nLastMissCacheExpiryCheck;
		quint32				m_nMissCacheExpiryInterval;
		quint32				m_nLastSettingsLoadTime;
		quint32				m_nSettingsLoadInterval;

		bool				m_bDebug;			// Loaded from settings. Enables additionnal debug output.

		bool				m_bUseMissCache;
		bool				m_bIsLoading;		// true during import operations. Used to avoid unnecessary regeneration of LiveList.
		bool				m_bNewRulesLoaded;	// true if new rules for sanity check have been loaded.
		bool				m_bDelayedSanityCheckRequested;
		quint32				m_nSanityCheckRequestTime;

		bool				m_bSaved;			// true if current security manager state already saved to file, false otherwise

		bool				m_bDenyPolicy;
		// m_bDenyPolicy == false : everything but specifically blocked IPs is allowed (default)
		// m_bDenyPolicy == true  : everything but specifically allowed IPs is rejected
		// Note that the default policy is only applied to IP related rules, as everything
		// else does not make any sense.

		/* ================================================================ */
		/* ========================= Construction ========================= */
		/* ================================================================ */
	public:
		CSecurity();
		~CSecurity();

		/* ================================================================ */
		/* ========================== Operations ========================== */
		/* ================================================================ */
		inline unsigned int	GetCount() const	{ return m_Rules.size();  }

		inline bool		DenyPolicy()			{ return m_bDenyPolicy; }
		void			DenyPolicy(bool bDenyPolicy);

		bool			Check(CSecureRule* pRule);
		void			MoveUp(CSecureRule* pRule);
		void			MoveDown(CSecureRule* pRule);
		void			Add(CSecureRule* pRule);
		void			Remove(CSecureRule* pRule);
		void			Clear();
		void			Expire();

		// Sanity check controll methods
		void			SanityCheck(bool bDelay = false);
		bool			LoadNewRules();
		bool			ClearNewRules();

		void			Ban(const QHostAddress* pAddress, BanLength nBanLength, bool bMessage = true, const QString& strComment = "");
//		void			Ban(const CShareazaFile* pFile, BanLength nBanLength, bool bMessage = true, const QString& strComment = "");

		// Methods used during sanity check
		bool			IsNewlyDenied(const QHostAddress* pAddress, const QString &source = "Unknown");
//		bool			IsNewlyDenied(const CQueryHit* pHit, const CQuerySearch* pQuery = NULL*/);

		bool			IsDenied(const QHostAddress* pAddress, const QString &source = "Unknown");
		// This does not check for the hit IP to avoid double checking.
//		bool			IsDenied(const CQueryHit* pHit, const CQuerySearch* pQuery = NULL);

		// Checks the user agent to see if it's a GPL breaker, or other trouble-maker
		// We don't ban them, but also don't offer leaf slots to them.
		bool			IsClientBad(const QString& sUserAgent) const;

		// Checks the user agent to see if it's a leecher client, or other banned client
		// Test new releases, and remove block if/when they are fixed.
		// Includes check of agent blocklist & agent security rules.
		bool			IsAgentBlocked(const QString& sUserAgent);

		// Check the evil's G1/G2 vendor code
		bool			IsVendorBlocked(const QString& sVendor) const;

		// Export/Import/Load/Save handlers
		void			LoadSettings(const quint32 tNow);
		bool			Load();
		bool			Save();
//		bool			Import(LPCTSTR pszFile);
//		CXMLElement*	ToXML();
//		bool			FromXML(CXMLElement* pXML);

	private:
		inline CIterator	GetEnd() const		{ return m_Rules.end();   }
		inline CIterator	GetIterator() const	{ return m_Rules.begin(); }

		inline bool		Debug() const			{ return m_bDebug; }

//		void			Serialize(CArchive& ar);

		// this returns the first rule found. Note that there might be others, too.
		CIterator		GetHash(const QString& sSHA1,
								const QString& sED2K,
								const QString& sTTH  = "",
								const QString& sBTIH = "",
								const QString& sMD5  = "") const;
		CIterator		GetUUID(const QUuid& oUUID) const;

		CIterator		Remove(CIterator i);

		bool			IsAgentDenied(const QString& strUserAgent);

		void			MissCacheAdd(const QString &sIP);
		void			MissCacheClear(const quint32 &tNow);
		void			EvaluateCacheUsage();				// determines whether it is logical to use the cache or not

		bool			IsDenied(const QString& strContent);
//		bool			IsDenied(const CShareazaFile* pFile);
//		bool			IsDenied(const CQuerySearch* pQuery, const QString& strContent);
	};
};

extern security::CSecurity Security;

#endif // SECURITY_H

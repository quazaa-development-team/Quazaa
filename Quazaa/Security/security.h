#ifndef SECURITY_H
#define SECURITY_H

#include <list>
#include <map>
#include <queue>
#include <set>

#include <QDomDocument>
#include <QHostAddress>
#include <QReadWriteLock>
#include <QString>
#include <QUuid>

#include "commonfunctions.h"
#include "file.h"
#include "securerule.h"
#include "time.h"

// Increment this if there have been made changes to the way of storing security rules.
#define SECURITY_CODE_VERSION	0
// History:
// 0 - Initial implementation

namespace security
{
	// typedef enum { tri_unknown, tri_true, tri_false } tristate;

	class CSecurity : public QObject
	{
		Q_OBJECT
		/* ================================================================ */
		/* ========================== Attributes ========================== */
		/* ================================================================ */
	public:
		static const QString xmlns;

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

		QReadWriteLock		m_pRWLock;

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

		// Security manager settings
		QString				m_sDataPath;				// Path to the security.dat file
		bool				m_bLogIPCheckHits;			// Post log message on IsDenied( QHostAdress ) call
		quint64				m_tRuleExpiryInterval;		// Check the security manager for expired hosts each x milliseconds
		quint64				m_tMissCacheExpiryInterval;	// Clear the miss cache each x ms

		// Timer IDs
		QUuid				m_idRuleExpiry;			// The ID of the signalQueue object.
		QUuid				m_idMissCacheExpiry;	// The ID of the signalQueue object.

		// Other
		bool				m_bUseMissCache;
		bool				m_bIsLoading;		// true during import operations. Used to avoid unnecessary GUI updates.
		bool				m_bNewRulesLoaded;	// true if new rules for sanity check have been loaded.
		unsigned short		m_nPendingOperations; // Counts the number of program modules that still need to call back after having finished a requested sanity check operation.

		bool				m_bSaved;			// true if current security manager state has already been saved to file, false otherwise

		bool				m_bDenyPolicy;
		// m_bDenyPolicy == false : everything but specifically blocked IPs is allowed (default)
		// m_bDenyPolicy == true  : everything but specifically allowed IPs is rejected
		// Note that the default policy is only applied to IP related rules, as everything
		// else does not make any sense.

	public:
		/* ================================================================ */
		/* ========================= Construction ========================= */
		/* ================================================================ */
		CSecurity();
		~CSecurity();

		/* ================================================================ */
		/* ========================== Operations ========================== */
		/* ================================================================ */
		inline unsigned int	getCount() const;

		inline bool		denyPolicy() const;
		void			setDenyPolicy(bool bDenyPolicy);

		bool			check(const CSecureRule* const pRule);
		void			add(CSecureRule* pRule);
		inline void		remove(CSecureRule* pRule);
		void			clear();

		void			ban(const QHostAddress& oAddress, BanLength nBanLength, bool bMessage = true, const QString& strComment = "");
		void			ban(const CFile& oFile, BanLength nBanLength, bool bMessage = true, const QString& strComment = "");

		// Methods used during sanity check
		bool			isNewlyDenied(const QHostAddress& oAddress);
		bool			isNewlyDenied(/*const CQueryHit* pHit,*/ const QList<QString>& lQuery);

		bool			isDenied(const QHostAddress& oAddress, const QString &source = "Unknown");
		// This does not check for the hit IP to avoid double checking.
		bool			isDenied(/*const CQueryHit* pHit, */const QList<QString>& lQuery);

		// Checks the user agent to see if it's a GPL breaker, or other trouble-maker
		// We don't ban them, but also don't offer leaf slots to them.
		bool			isClientBad(const QString& sUserAgent) const;

		// Checks the user agent to see if it's a leecher client, or other banned client
		// Test new releases, and remove block if/when they are fixed.
		// Includes check of agent blocklist & agent security rules.
		bool			isAgentBlocked(const QString& sUserAgent);

		// Check the evil's G1/G2 vendor code
		bool			isVendorBlocked(const QString& sVendor) const;

		// Export/Import/Load/Save handlers
		bool			start(); // connects signals etc.
		bool			stop(); // makes the Security Manager ready for destruction
		bool			load();
		bool			save(bool bForceSaving = false);
		bool			import(const QString& sPath);
		QDomDocument	toXML();
		bool			fromXML(const QDomDocument& oXMLroot);

	signals:
		void			ruleAdded(QSharedPointer<CSecureRule> pRule);
		void			ruleRemoved(QSharedPointer<CSecureRule> pRule);

		// This is used to inform other modules that a system wide sanity check has become necessary.
		void			performSanityCheck();

	public slots:
		// Start system wide sanity check
		void			sanityCheck();
		// This slot must be triggered by all listeners to performSanityCheck() once they have completed their work.
		void			sanityCheckPerformed();
		// Aborts all currently running sanity checks by clearing their rule lists.
		void			forceEndOfSanityCheck();

		void			expire();
		void			missCacheClear();

		// Trigger this slot to inform the security manager about changes in the security settings.
		void			settingsChanged();

	private:
		// Sanity check helper methods
		void			loadNewRules();
		void			clearNewRules();

		bool			load(QString sPath);

		// this returns the first rule found. Note that there might be others, too.
		CIterator		getHash(const QList< CHash >& hashes) const;
		CIterator		getUUID(const QUuid& oUUID) const;

		// Use bLockRequired to enable/disable locking inside function.
		inline void		remove(CSecureRule* pRule, bool bLockRequired);
		void			remove(CIterator i);

		bool			isAgentDenied(const QString& strUserAgent);

		void			missCacheAdd(const QString& sIP);
		void			missCacheClear(bool bRefreshInterval);
		void			evaluateCacheUsage();				// determines whether it is logical to use the cache or not

		bool			isDenied(const QString& strContent);
		bool			isDenied(const CFile& oFile);
		bool			isDenied(const QList<QString>& lQuery, const QString& strContent);
	};

	unsigned int CSecurity::getCount() const
	{
		return m_Rules.size();
	}

	bool CSecurity::denyPolicy() const
	{
		return m_bDenyPolicy;
	}

	// publically available
	void CSecurity::remove(CSecureRule* pRule)
	{
		if ( !pRule )
			return;

		remove( pRule, true );
	}

	// for internal use only
	void CSecurity::remove(CSecureRule* pRule, bool bLockRequired)
	{
		if ( bLockRequired )
			m_pRWLock.lockForWrite();

		remove( getUUID( pRule->m_oUUID ) );

		if ( bLockRequired )
			m_pRWLock.unlock();
	}
}

extern security::CSecurity securityManager;

#endif // SECURITY_H

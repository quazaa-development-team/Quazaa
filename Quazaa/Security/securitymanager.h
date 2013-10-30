/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2013.
** This file is part of the Quazaa Security Library (quazaa.sourceforge.net)
**
** The Quazaa Security Library is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** The Quazaa Security Library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with the Quazaa Security Library; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef SECURITYMANAGER_H
#define SECURITYMANAGER_H

#include <QList>
#include <QQueue>
#include <QTimer>

// Increment this if there have been made changes to the way of storing security rules.
#define SECURITY_CODE_VERSION 1
// History:
// 0 - Initial implementation

#include "securerule.h"
#include "contentrule.h"
#include "hashrule.h"
#include "iprangerule.h"
#include "iprule.h"
#include "regexprule.h"
#include "useragentrule.h"
#include "commonfunctions.h"

// DODO: Add quint16 GUI ID to rules and update GUI only when there is a change to the rule.
// TODO: Enable/disable this according to the visibility within the GUI
// TODO: m_nMaxUnsavedRules >> Settings
// TODO: add log calls + defines to enable/disable
// TODO: user agent blocking case insensitive + partial matching

class CSecurity : public QObject
{
	Q_OBJECT

public:
	static const QString xmlns;
	static const char* ruleInfoSignal;

private:
	// contains all rules
	QList<CSecureRule*>   m_lRules;

	// Used to manage newly added rules during sanity check
	QList<CSecureRule*>   m_lLoadedAddressRules;
	QQueue<CSecureRule*>      m_newAddressRules;
	QList<CSecureRule*>   m_lLoadedHitRules;
	QQueue<CSecureRule*>      m_newHitRules;

	// IP rule miss cache
	QSet<uint>          m_Cache;

	// single IP blocking rules
	QList<CIPRule*>     m_lIPs;

	// multiple IP blocking rules
	QList<CIPRangeRule*>    m_lIPRanges;

	// hash rules
	// Note: Using a multimap eliminates eventual problems of hash
	// collisions caused by weaker hashes like MD5 for example.
	QMultiMap<uint, CHashRule*>        m_lHashes;

	// all other content rules
	QList<CContentRule*>    m_lContents;

	// RegExp rules
	QList<CRegularExpressionRule*>     m_lRegularExpressions;

	// User agent rules
	QMap<QString, CUserAgentRule*>   m_UserAgents;

	// Security manager settings
	bool                m_bLogIPCheckHits;          // Post log message on IsDenied( QHostAdress ) call

	QTimer* m_tMaintenance;

#ifdef _DEBUG // use failsafe to abort sanity check only in debug version
	QUuid               m_idForceEoSC;        // The signalQueue ID (force end of sanity check)
#endif

	// Other
	bool                m_bUseMissCache;
	bool                m_bIsLoading;         // true during import operations. Used to avoid unnecessary GUI updates.
	bool                m_bNewRulesLoaded;    // true if new rules for sanity check have been loaded.
	unsigned short      m_nPendingOperations; // Counts the number of program modules that still need to call back after having finished a requested sanity check operation.

	quint16             m_nMaxUnsavedRules;   // maximal number of unsaved rules to tolerate before forcing save
	mutable QAtomicInt  m_nUnsaved;           // count of unsaved rules

	bool                m_bDenyPolicy;
	// m_bDenyPolicy == false : everything but specifically blocked IPs is allowed (default)
	// m_bDenyPolicy == true  : everything but specifically allowed IPs is rejected
	// Note that the default policy is only applied to IP related rules, as everything
	// else does not make any sense.

public:
	CSecurity();
	~CSecurity();

	inline quint32  getCount() const;

	inline bool     denyPolicy() const;
	void            setDenyPolicy(bool bDenyPolicy);

	bool            check(const CSecureRule* const pRule) const;
	void            add(CSecureRule* pRule);
	// Use bLockRequired to enable/disable locking inside function.
	inline void     remove(CSecureRule* pRule);
	void            clear();

	void            ban(const CEndPoint &oAddress, RuleTime::Time nRuleTime, bool bMessage = true, const QString& sComment = "", bool bAutomatic = true, bool bForever = false);
//	void            ban(const CFile& oFile, BanLength nRuleTime, bool bMessage = true, const QString& sComment = "");

	// Methods used during sanity check
	bool            isNewlyDenied(const CEndPoint& oAddress);
	bool            isNewlyDenied(const CQueryHit* pHit, const QList<QString>& lQuery);

	bool            isDenied(const CEndPoint& oAddress);
	// This does not check for the hit IP to avoid double checking.
	bool            isDenied(const CQueryHit* const pHit, const QList<QString>& lQuery);
	bool isPrivate(const CEndPoint &oAddress);
	CIPRule *isInAddressRules(const CEndPoint nIp);
	CIPRangeRule *isInAddressRangeRules(const CEndPoint nIp);

	// Checks the user agent to see if it's a GPL breaker, or other trouble-maker
	// We don't ban them, but also don't offer leaf slots to them.
	bool            isClientBad(const QString& sUserAgent) const;

	// Checks the user agent to see if it's a leecher client, or other banned client
	// Test new releases, and remove block if/when they are fixed.
	// Includes check of agent blocklist & agent security rules.
	bool            isAgentBlocked(const QString& sUserAgent);

	// Check the evil's G1/G2 vendor code
	bool            isVendorBlocked(const QString& sVendor) const;

	// Export/Import/Load/Save handlers
	bool            start(); // connects signals etc.
	bool            stop(); // makes the Security Manager ready for destruction
	bool            load();
	bool            save(bool bForceSaving = false) const;
	static quint32  writeToFile(const void* const pManager, QFile& oFile); // used by save()
	bool            import(const QString& sPath);
	bool            toXML(const QString& sPath) const;
	bool            fromXML(const QString& sPath);
	bool			fromP2P(const QString& sFile);

	// Allows for external callers to find out about how many listeners there
	// are to the Security Manager Signals.
	int             receivers(const char* signal) const;

signals:
	void            ruleAdded(CSecureRule* pRule);
	void            ruleRemoved(CSecureRule* pRule);

	void            ruleInfo(CSecureRule* pRule);

	void            securityHit();

	// This is used to inform other modules that a system wide sanity check has become necessary.
	void            performSanityCheck();
	void			updateLoadMax(int max);
	void			updateLoadProgress(int progress);

public slots:
	// Trigger this to let the Security Manager emit all rules
	void            requestRuleList();

	// Start system wide sanity check
	void            sanityCheck();
	// This slot must be triggered by all listeners to performSanityCheck() once they have completed their work.
	void            sanityCheckPerformed();
	// Aborts all currently running sanity checks by clearing their rule lists.
	void            forceEndOfSanityCheck();

	void            expire();
	void            missCacheClear();

	// Trigger this slot to inform the security manager about changes in the security settings.
	void            settingsChanged();

private:
	// Sanity check helper methods
	void            loadNewRules();
	void            clearNewRules();

	bool            load(QString sPath);

	// this returns the first rule found. Note that there might be others, too.
	CHashRule *getHash(const QList< CHash >& hashes) const;
	CSecureRule *getUUID(const QUuid& oUUID) const;

	bool            isAgentDenied(const QString& sUserAgent);

	void            missCacheAdd(const uint& nIP);
	void            evaluateCacheUsage();				// determines whether it is logical to use the cache or not

	bool            isDenied(const QString& sContent);
	bool            isDenied(const CQueryHit* const pHit);
	bool            isDenied(const QList<QString>& lQuery, const QString& sContent);

	inline void     hit(CSecureRule *pRule);
};

quint32 CSecurity::getCount() const
{
	return (quint32)m_lRules.size();
}

bool CSecurity::denyPolicy() const
{
	return m_bDenyPolicy;
}

void CSecurity::hit(CSecureRule* pRule)
{
	if(!pRule->isBeingRemoved()) {
		pRule->count();
		emit securityHit();
	}
}

extern CSecurity securityManager;

#endif // SECURITYMANAGER_H

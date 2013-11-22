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
	static const QString			xmlns;
	static const char*				ruleInfoSignal;

private:
	QMutex							m_pSection;				// Used to lock operations while lists are being modified, added to or checked
	bool							m_bIsLoading;			// true during import operations. Used to avoid unnecessary GUI updates.

	QList<CSecureRule*>				m_lRules;			// contains all rules
	// Used to manage newly added rules during sanity check
	QList<CSecureRule*>				m_lLoadedAddressRules;
	QQueue<CSecureRule*>			m_lqNewAddressRules;
	QList<CSecureRule*>				m_lLoadedHitRules;
	QQueue<CSecureRule*>			m_lqNewHitRules;
	QSet<uint>						m_lsCache;				// IP rule miss cache
	QList<CIPRule*>					m_lIPs;					// single IP blocking rules
	QList<CIPRangeRule*>			m_lIPRanges;			// multiple IP blocking rules
	QMultiMap<uint, CHashRule*>		m_lmmHashes;				// hash rules
	// Note: Using a multimap eliminates eventual problems of hash
	// collisions caused by weaker hashes like MD5 for example.
	QList<CContentRule*>			m_lContents;			// all other content rules
	QList<CRegularExpressionRule*>	m_lRegularExpressions;	// RegExp rules
	QMap<QString, CUserAgentRule*>	m_lmUserAgents;			// User agent rules
	// Security manager settings
	bool							m_bLogIPCheckHits;		// Post log message on IsDenied( QHostAdress ) call
	QTimer*							m_tMaintenance;			// This timer runs the maintenance tasks every second
#ifdef _DEBUG // use failsafe to abort sanity check only in debug version
	QUuid							m_idForceEoSC;			// The signalQueue ID (force end of sanity check)
#endif
	bool							m_bUseMissCache;
	bool							m_bNewRulesLoaded;		// true if new rules for sanity check have been loaded.
	unsigned short					m_nPendingOperations;	// Counts the number of program modules that still need to call back after having finished a requested sanity check operation.
	quint16							m_nMaxUnsavedRules;		// maximal number of unsaved rules to tolerate before forcing save
	mutable QAtomicInt				m_nUnsaved;				// count of unsaved rules
	bool							m_bDenyPolicy;
	// m_bDenyPolicy == false : everything but specifically blocked IPs is allowed (default)
	// m_bDenyPolicy == true  : everything but specifically allowed IPs is rejected
	// Note that the default policy is only applied to IP related rules, as everything
	// else does not make any sense.

public:
	CSecurity();
	~CSecurity();

	inline quint32  getCount() const;
	inline bool     denyPolicy() const;
	void			setDenyPolicy(bool bDenyPolicy);
	bool			check(const CSecureRule* const pRule) const;
	bool			add(CSecureRule* pRule);
	void			remove(CSecureRule* pRule);
	void			clear();
	void			ban(const CEndPoint &oAddress, quint32 nRuleTime, bool bMessage = true, const QString& sComment = "", bool bAutomatic = true, bool bForever = false);
	// Methods used during sanity check
	bool			isNewlyDenied(const CEndPoint& oAddress);
	bool			isNewlyDenied(const CQueryHit* pHit, const QList<QString>& lQuery);
	bool			isDenied(const CEndPoint& oAddress);
	bool			isDenied(const CQueryHit* const pHit, const QList<QString>& lQuery);	// This does not check for the hit IP to avoid double checking.
	bool			isPrivate(const CEndPoint &oAddress);
	CIPRule*		isInAddressRules(const CEndPoint nIp);
	CIPRangeRule*	isInAddressRangeRules(const CEndPoint nIp);
	// Checks the user agent to see if it's a GPL breaker, or other trouble-maker
	// We don't ban them, but also don't offer leaf slots to them.
	bool			isClientBad(const QString& sUserAgent) const;
	// Checks the user agent to see if it's a leecher client, or other banned client
	// Test new releases, and remove block if/when they are fixed.
	// Includes check of agent blocklist & agent security rules.
	bool			isAgentBlocked(const QString& sUserAgent);
	bool			isVendorBlocked(const QString& sVendor) const;							// Check the evil's G1/G2 vendor code
	// Export/Import/Load/Save handlers
	bool			start();																// connects signals etc.
	bool			stop();																	// makes the Security Manager ready for destruction
	bool			load();
	bool			save(bool bForceSaving = false) const;
	static quint32	writeToFile(const void* const pManager, QFile& oFile); // used by save()
	bool			import(const QString& sPath);
	bool			toXML(const QString& sPath) const;
	bool			fromXML(const QString& sPath);
	bool			fromP2P(const QString& sFile);
	int				receivers(const char* signal) const;	// Allows for external callers to find out about how many listeners there are to the Security Manager Signals.

signals:
	void			ruleAdded(CSecureRule* pRule);
	void			ruleRemoved(CSecureRule* pRule);
	void			ruleInfo(CSecureRule* pRule);
	void			securityHit();
	void			performSanityCheck();	// This is used to inform other modules that a system wide sanity check has become necessary.
	void			updateLoadMax(int max);
	void			updateLoadProgress(int progress);

public slots:
	void            requestRuleList();			// Trigger this to let the Security Manager emit all rules
	void			sanityCheck();				// Start system wide sanity check
	void			sanityCheckPerformed();		// This slot must be triggered by all listeners to performSanityCheck() once they have completed their work.
	void			forceEndOfSanityCheck();	// Aborts all currently running sanity checks by clearing their rule lists.
	void			expire();
	void			missCacheClear();
	void			settingsChanged();			// Trigger this slot to inform the security manager about changes in the security settings.

private:	// Sanity check helper methods
	void			loadNewRules();
	void			clearNewRules();
	bool			load(QString sPath);
	CHashRule		*getHash(const QList< CHash >& hashes) const;	// this returns the first rule found. Note that there might be others, too.
	CSecureRule		*getUUID(const QUuid& oUUID) const;
	bool			isAgentDenied(const QString& sUserAgent);
	void			missCacheAdd(const uint& nIP);
	void			evaluateCacheUsage();				// determines whether it is logical to use the cache or not
	bool			isDenied(const QString& sContent);
	bool			isDenied(const CQueryHit* const pHit);
	bool			isDenied(const QList<QString>& lQuery, const QString& sContent);
	inline void		hit(CSecureRule *pRule);
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

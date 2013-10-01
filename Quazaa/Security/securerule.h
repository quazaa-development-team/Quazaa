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

#ifndef SECURERULE_H
#define SECURERULE_H

#include <list>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#if QT_VERSION >= 0x050000
#  include <QRegularExpression>
#else
#  include <QRegExp>
#endif

#include <QString>
#include <QUuid>
#include <QAtomicInt>

#if SECURITY_ENABLE_GEOIP
#include "geoiplist.h"
#endif

#include "NetworkCore/endpoint.h"
#include "NetworkCore/Hashes/hash.h"
#include "NetworkCore/queryhit.h"

// Note: The locking information within the doxygen comments refers to the RW lock of the Security
//       Manager.

namespace Security
{

typedef quint16 TRuleID; // used for GUI updating

class CSecureRule
{
public:
	typedef enum { srContentUndefined = 0, srContentAddress = 1, srContentAddressRange = 2, srContentCountry = 3,
				   srContentHash = 4, srContentRegExp = 5, srContentUserAgent = 6, srContentText = 7 } TRuleType;

	typedef enum { srNull = 0, srAccept = 1, srDeny = 2 } TPolicy;
	enum { srIndefinite = 0, srSession = 1 };

protected:
	// Type is critical to functionality and may not be changed externally.
	TRuleType   m_nType;

	// Contains a string representation of the rule content for faster GUI accesses.
	// Can be accessed via getContentString().
	QString     m_sContent;

private:
	// Hit counters
	QAtomicInt  m_nToday;
	QAtomicInt  m_nTotal;

	// List of pointers that will be set to 0 if this instance of CSecureRule is deleted.
	// Note that the content of this list is not forwarded to copies of this rule.
	std::list<CSecureRule**> m_lPointers;

public:
	TPolicy     m_nAction;
	QUuid       m_oUUID;
	quint32     m_tExpire;
	QString     m_sComment;

public:
	// Construction / Destruction
	CSecureRule();
	// See m_lPointers for why we don't use the default copy constructor.
	CSecureRule(const CSecureRule& pRule);
	virtual ~CSecureRule();

	// Returns a copy of the current rule. Note that this copy does not contain
	// any information on pointers registered to the original CSecureRule object.
	virtual CSecureRule* getCopy() const;

	// Operators
	virtual bool    operator==(const CSecureRule& pRule) const;
	bool            operator!=(const CSecureRule& pRule) const;

	virtual bool    parseContent(const QString& sContent);
	QString         getContentString() const;

	// Registers a pointer to a Secure Rule to assure it is set to NULL if the Secure
	// Rule is deleted. Note that a pointer who has been registered needs to be unregistered
	// before freeing its memory.
	void            registerPointer(CSecureRule** pRule);
	// Call this before removing a pointer you have previously registered.
	void            unRegisterPointer(CSecureRule** pRule);

	inline bool     isExpired(quint32 tNow, bool bSession = false) const;
	inline quint32  getExpiryTime() const;

	// Hit count control
	inline void     count();
	inline void     resetCount();
	inline quint32  getTodayCount() const;
	inline quint32  getTotalCount() const;
	inline void     loadTotalCount(quint32 nTotal);

	// get the rule type
	inline TRuleType type() const;

	// Check content for hits
	virtual bool    match(const CEndPoint& oAddress) const;
	virtual bool    match(const QString& sContent) const;
	virtual bool    match(const CQueryHit* const pHit) const;
	virtual bool    match(const QList<QString>& lQuery, const QString& sContent) const;

	// Read/write rule from/to file
	static void     load(CSecureRule*& pRule, QDataStream& fsFile, const int nVersion);
	static void     save(const CSecureRule* const pRule, QDataStream& oStream);

	// XML Import/Export functionality
	static CSecureRule* fromXML(QXmlStreamReader& oXMLdocument, float nVersion);
	inline virtual void toXML(QXmlStreamWriter& oXMLdocument) const;

protected:
	// Contains default code for XML generation.
	static void         toXML(const CSecureRule& oRule, QXmlStreamWriter& oXMLdocument);
};

/**
 * @brief CSecureRule::isExpired allows to check whether a rule has expired.
 * @param tNow indicates the current time in seconds since 1.1.1970UTC
 * @param bSession indicates whether this is the end of the session/start of a new session. In both
 * cases, set this to true and the return value for session ban rules will be true.
 * @return true if the rule has expired, false otherwise
 */
bool CSecureRule::isExpired(quint32 tNow, bool bSession) const
{
	if ( m_tExpire == srIndefinite ) return false;
	if ( m_tExpire == srSession ) return bSession;
	return m_tExpire < tNow;
}

/**
 * @brief CSecureRule::getExpiryTime allows to access the expiry time of a rule.
 * @return srIndefinite = 0, srSession = 1 or the time in seconds since 1.1.1970UTC when the rule
 * will/has expire(d)
 */
quint32 CSecureRule::getExpiryTime() const
{
	return m_tExpire;
}

/**
 * @brief CSecureRule::count increases the total and today hit counters by one each.
 * Requires Locking: /
 */
void CSecureRule::count()
{
	m_nToday.fetchAndAddOrdered(1);
	m_nTotal.fetchAndAddOrdered(1);
}

/**
 * @brief CSecureRule::resetCount resets total and today hit counters to 0.
 * Requires Locking: /
 */
void CSecureRule::resetCount()
{
	m_nToday.fetchAndStoreOrdered(0);
	m_nTotal.fetchAndAddOrdered(0);
}

/**
 * @brief CSecureRule::getTodayCount allows to access the today hit counter.
 * @return the value of the today hit counter.
 * Requires Locking: /
 */
quint32 CSecureRule::getTodayCount() const
{
	return m_nToday.loadAcquire();
}

/**
 * @brief CSecureRule::getTotalCount allows to access the total hit counter.
 * @return the value of the total hit counter.
 * Requires Locking: /
 */
quint32 CSecureRule::getTotalCount() const
{
	return m_nTotal.loadAcquire();
}

/**
 * @brief CSecureRule::loadTotalCount allows to access the total hit counter.
 * @param nTotal the new value of the total hit counter.
 * Requires Locking: /
 */
void CSecureRule::loadTotalCount( quint32 nTotal )
{
	m_nTotal.storeRelease(nTotal);
}

/**
 * @brief CSecureRule::type allows to access the type of this rule.
 * @return the rule type.
 * Requires Locking: R
 */
CSecureRule::TRuleType CSecureRule::type() const
{
	return m_nType;
}

void CSecureRule::toXML( QXmlStreamWriter& ) const {}

/*  ------------------------------------------------------------ */
/*  ------------------------- CIPRule -------------------------- */
/*  ------------------------------------------------------------ */

class CIPRule : public CSecureRule
{
private:
	QHostAddress m_oIP;

public:
	CIPRule();

	inline QHostAddress IP() const;
	inline void			setIP( const QHostAddress& oIP );

	bool				parseContent(const QString& sContent);

	inline CSecureRule*	getCopy() const;

	bool				match(const CEndPoint& oAddress) const;
	void				toXML(QXmlStreamWriter& oXMLdocument) const;

};

QHostAddress CIPRule::IP() const
{
	return m_oIP;
}

void CIPRule::setIP( const QHostAddress& oIP )
{
	m_oIP = oIP;
	m_sContent = oIP.toString();
}

CSecureRule* CIPRule::getCopy() const
{
	return new CIPRule( *this );
}

/*  ------------------------------------------------------------ */
/*  ---------------------- CIPRangeRule ------------------------ */
/*  ------------------------------------------------------------ */

class CIPRangeRule : public CSecureRule
{
private:
	QPair<QHostAddress, int> m_oSubNet;

public:
	CIPRangeRule();

	inline QHostAddress	IP() const;
	inline int			mask() const;

	bool				parseContent(const QString& sContent);

	inline CSecureRule*	getCopy() const;

	inline bool			match(const CEndPoint& oAddress) const;
	void				toXML(QXmlStreamWriter& oXMLdocument) const;
};

QHostAddress CIPRangeRule::IP() const
{
	return m_oSubNet.first;
}

int CIPRangeRule::mask() const
{
	return m_oSubNet.second;
}

CSecureRule* CIPRangeRule::getCopy() const
{
	return new CIPRangeRule( *this );
}

bool CIPRangeRule::match(const CEndPoint& oAddress) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentAddressRange );
#endif //_DEBUG

	return oAddress.isInSubnet( m_oSubNet );
}

/*  ------------------------------------------------------------ */
/*  ---------------------- CCountryRule ------------------------ */
/*  ------------------------------------------------------------ */
#if SECURITY_ENABLE_GEOIP
class CCountryRule : public CSecureRule
{
public:
	CCountryRule();

	bool				parseContent(const QString& sContent);

	inline CSecureRule*	getCopy() const;

	bool				match(const CEndPoint& oAddress) const;
	void				toXML(QXmlStreamWriter& oXMLdocument) const;
};

CSecureRule* CCountryRule::getCopy() const
{
	return new CCountryRule( *this );
}
#endif // SECURITY_ENABLE_GEOIP

/*  ------------------------------------------------------------ */
/*  ------------------------ CHashRule ------------------------- */
/*  ------------------------------------------------------------ */

class CHashRule : public CSecureRule
{
private:
	QMap< CHash::Algorithm, CHash >	m_Hashes;

public:
	CHashRule();

	QList< CHash >		getHashes() const;
	void				setHashes(const QList< CHash >& hashes);

	bool				parseContent(const QString& sContent);

	inline CSecureRule*	getCopy() const;

	bool				hashEquals(CHashRule& oRule) const;

	bool				match(const CQueryHit* const pHit) const;
	bool				match(const QList<CHash>& lHashes) const;

	void				toXML(QXmlStreamWriter& oXMLdocument) const;
};

CSecureRule* CHashRule::getCopy() const
{
	return new CHashRule( *this );
}

/*  ------------------------------------------------------------ */
/*  ----------------------- CRegExpRule ------------------------ */
/*  ------------------------------------------------------------ */

// There are two kinds of rules:1. Those which contain <_>, <1>...<9> or <>
//								2. All other rules.
class CRegExpRule : public CSecureRule
{
private:
	bool				m_bSpecialElements;

#if QT_VERSION >= 0x050000
	QRegularExpression	m_regularExpressionContent;
#else
	QRegExp				m_regExpContent;
#endif

public:
	CRegExpRule();

	bool				operator==(const CSecureRule& pRule) const;

	bool				parseContent(const QString& sContent);

	inline CSecureRule*	getCopy() const;

	bool				match(const QList<QString>& lQuery, const QString& sContent) const;
	void				toXML(QXmlStreamWriter& oXMLdocument) const;

private:
	static bool			replace(QString& sReplace, const QList<QString>& lQuery, quint8& nCurrent);
};

CSecureRule* CRegExpRule::getCopy() const
{
	return new CRegExpRule( *this );
}

/*  ------------------------------------------------------------ */
/*  ---------------------- CUserAgentRule ---------------------- */
/*  ------------------------------------------------------------ */

class CUserAgentRule : public CSecureRule
{
private:
	bool				m_bRegExp;  // is the content of this rule is a regular expression?

#if QT_VERSION >= 0x050000
	QRegularExpression	m_regularExpressionContent;
#else
	QRegExp				m_regExpContent;
#endif

public:
	CUserAgentRule();

	bool				operator==(const CSecureRule& pRule) const;

	void				setRegExp(bool bRegExp);
	inline bool			getRegExp() const;

	bool				parseContent(const QString& sContent);

	inline CSecureRule*	getCopy() const;

	bool				match(const QString& sUserAgent) const;
	bool				partialMatch(const QString &sUserAgent) const;
	void				toXML(QXmlStreamWriter& oXMLdocument) const;
};

CSecureRule* CUserAgentRule::getCopy() const
{
	return new CUserAgentRule( *this );
}

bool CUserAgentRule::getRegExp() const
{
	return m_bRegExp;
}

/*  ------------------------------------------------------------ */
/*  ----------------------- CContentRule ----------------------- */
/*  ------------------------------------------------------------ */

// contains everyting that does not fit into the other rule classes
class CContentRule : public CSecureRule
{
private:
	bool				m_bAll;
	QList< QString >	m_lContent;

	typedef QList< QString >::const_iterator CListIterator;

public:
	CContentRule();

	bool				operator==(const CSecureRule& pRule) const;

	inline void			setAll(bool all = true);
	inline bool			getAll() const;

	bool				parseContent(const QString& sContent);

	inline CSecureRule*	getCopy() const;

	bool				match(const QString& sFileName) const;
	bool				match(const CQueryHit* const pHit) const;

	void				toXML(QXmlStreamWriter& oXMLdocument) const;
};

CSecureRule* CContentRule::getCopy() const
{
	return new CContentRule( *this );
}

void CContentRule::setAll(bool all)
{
	m_bAll = all;
}

bool CContentRule::getAll() const
{
	return m_bAll;
}

}

#endif // SECURERULE_H

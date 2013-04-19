#ifndef SECURERULE_H
#define SECURERULE_H

#include <list>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QHostAddress>

#if QT_VERSION >= 0x050000
#  include <QRegularExpression>
#else
#  include <QRegExp>
#endif

#include <QString>
#include <QUuid>

#include "NetworkCore/Hashes/hash.h"
#include "NetworkCore/queryhit.h"

namespace Security
{

class CSecureRule
{
public:
	typedef enum { srContentUndefined = 0, srContentAddress = 1, srContentAddressRange = 2, srContentCountry = 3,
				   srContentHash = 4, srContentRegExp = 5, srContentUserAgent = 6, srContentText = 7 } RuleType;

	typedef enum { srNull = 0, srAccept = 1, srDeny = 2 } Policy;
	enum { srIndefinite = 0, srSession = 1 };

protected:
	// Type is critical to functionality and may not be changed externally.
	RuleType	m_nType;

	// Contains a string representation of the rule content for faster GUI accesses.
	// Can be accessed via getContentString().
	QString		m_sContent;

private:
	// Hit counters
	quint32		m_nToday;
	quint32		m_nTotal;

	// List of pointers that will be set to 0 if this instance of CSecureRule is deleted.
	// Note that the content of this list is not forwarded to copies of this rule.
	std::list<CSecureRule**> m_lPointers;

public:
	Policy		m_nAction;
	QUuid		m_oUUID;
	quint32		m_tExpire;
	QString		m_sComment;

public:
	// Construction / Destruction
	CSecureRule(bool bCreate = true);
	// See m_lPointers for why we don't use the default copy constructor.
	CSecureRule(const CSecureRule& pRule);
	virtual			~CSecureRule();

	// Operators
	virtual bool            operator==(const CSecureRule& pRule) const;
	bool					operator!=(const CSecureRule& pRule) const;

	virtual bool			parseContent(const QString& sContent);
	QString					getContentString() const;

	// Returns a copy of the current rule. Note that this copy does not contain
	// any information on pointers registered to the original CSecureRule object.
	virtual CSecureRule*	getCopy() const;

	// Registers a pointer to a Secure Rule to assure it is set to NULL if the Secure
	// Rule is deleted. Note that a pointer who has been registered needs to be unregistered
	// before freeing its memory.
	void			registerPointer(CSecureRule** pRule);
	// Call this before removing a pointer you have previously registered.
	void			unRegisterPointer(CSecureRule** pRule);

	bool			isExpired(quint32 nNow, bool bSession = false) const;

	// Hit count control
	inline void		count();
	inline void		resetCount();
	inline quint32	getTodayCount() const;
	inline quint32	getTotalCount() const;
	inline void		loadTotalCount(quint32 nTotal);

	// get the rule type
	inline RuleType	type() const;

	// Check content for hits
	virtual bool	match(const QHostAddress& oAddress) const;
	virtual bool	match(const QString& sContent) const;
	virtual bool    match(const CQueryHit* const pHit) const;
	virtual bool	match(const QList<QString>& lQuery, const QString& sContent) const;

	// Read/write rule from/to file
	static void		load(CSecureRule*& pRule, QDataStream& fsFile, const int nVersion);
	static void     save(const CSecureRule* const pRule, QDataStream& oStream);

	// XML Import/Export functionality
	static CSecureRule*	fromXML(QXmlStreamReader& oXMLdocument, float nVersion);
	inline virtual void	toXML(QXmlStreamWriter& oXMLdocument) const;

protected:
	// Contains default code for XML generation.
	static void			toXML(const CSecureRule& oRule, QXmlStreamWriter& oXMLdocument);
};

void CSecureRule::count()
{
	m_nToday++;
	m_nTotal++;
}

void CSecureRule::resetCount()
{
	m_nToday = m_nTotal = 0;
}

quint32 CSecureRule::getTodayCount() const
{
	return m_nToday;
}

quint32 CSecureRule::getTotalCount() const
{
	return m_nTotal;
}

void CSecureRule::loadTotalCount( quint32 nTotal )
{
	m_nTotal = nTotal;
}

CSecureRule::RuleType CSecureRule::type() const
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
	CIPRule(bool bCreate = true);

	inline QHostAddress IP() const;
	inline void			setIP( const QHostAddress& oIP );

	bool				parseContent(const QString& sContent);

	inline CSecureRule*	getCopy() const;

	bool				match(const QHostAddress& oAddress) const;
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
	CIPRangeRule(bool bCreate = true);

	inline QHostAddress	IP() const;
	inline int			mask() const;

	bool				parseContent(const QString& sContent);

	inline CSecureRule*	getCopy() const;

	inline bool			match(const QHostAddress& oAddress) const;
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

bool CIPRangeRule::match(const QHostAddress &oAddress) const
{
#ifdef _DEBUG
	Q_ASSERT( m_nType == srContentAddressRange );
#endif //_DEBUG

	return oAddress.isInSubnet( m_oSubNet );
}

/*  ------------------------------------------------------------ */
/*  ---------------------- CCountryRule ------------------------ */
/*  ------------------------------------------------------------ */

class CCountryRule : public CSecureRule
{
public:
	CCountryRule(bool bCreate = true);

	bool				parseContent(const QString& sContent);

	inline CSecureRule*	getCopy() const;

	bool				match(const QHostAddress& oAddress) const;
	void				toXML(QXmlStreamWriter& oXMLdocument) const;
};

CSecureRule* CCountryRule::getCopy() const
{
	return new CCountryRule( *this );
}

/*  ------------------------------------------------------------ */
/*  ------------------------ CHashRule ------------------------- */
/*  ------------------------------------------------------------ */

class CHashRule : public CSecureRule
{
private:
	QMap< CHash::Algorithm, CHash >	m_Hashes;

public:
	CHashRule(bool bCreate = true);

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
	CRegExpRule(bool bCreate = true);

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
	CUserAgentRule(bool bCreate = true);

	bool				operator==(const CSecureRule& pRule) const;

	void				setRegExp(bool bRegExp);
	inline bool			getRegExp() const;

	bool				parseContent(const QString& sContent);

	inline CSecureRule*	getCopy() const;

	bool				match(const QString& sUserAgent) const;
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
	CContentRule(bool bCreate = true);

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

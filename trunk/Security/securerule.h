#ifndef SECURERULE_H
#define SECURERULE_H

#include <QDomElement>
#include <QHostAddress>
#include <QRegExp>
#include <QString>
#include <QUuid>

#include "ShareManager/file.h"



// TODO:
// * Move inlines outside of class definition.
// * Make operator== work for booleans
// * Check if content string is set correctly in each rule type



namespace security
{

class CSecureRule
{
public:
	typedef enum { srContentUndefined = 0, srContentAddress = 1, srContentAddressRange = 2, srContentCountry = 3,
				   srContentHash = 4, srContentRegExp = 5, srContentUserAgent = 6, srContentOther = 7 } RuleType;

	typedef enum { srNull, srAccept, srDeny } Policy;
	enum { srIndefinite = 0, srSession = 1 };

protected:
	// Type is critical to functionality and may not be changed externally.
	RuleType	m_nType;
	QString		m_sContent;

private:
	// Hit counters
	quint32		m_nToday;
	quint32		m_nTotal;

public:
	Policy		m_nAction;
	QUuid		m_oUUID;
	quint32		m_tExpire;
	QString		m_sComment;

public:
	// Construction / Destruction
	CSecureRule(bool bCreate = true);
	virtual inline	~CSecureRule() {}

	// Operators
	bool			operator==(const CSecureRule& pRule);
	bool			operator!=(const CSecureRule& pRule);

	virtual CSecureRule*	getCopy() const;
	QString			getContentString() const;

	bool			isExpired(quint32 nNow, bool bSession = false) const;

	// Hit count control
	inline void		count();
	inline void		resetCount();
	inline quint32	getTodayCount() const;
	inline quint32	getTotalCount() const;
	inline void		loadTotalCount( quint32 nTotal );

	// get the rule type
	inline RuleType	getType() const;

	// Check content for hits
	virtual bool	match(const QHostAddress& oAddress) const;
	virtual bool	match(const QString& sContent) const;
	virtual bool	match(const CFile& oFile) const;
	virtual bool	match(const QList<QString>& lQuery, const QString& strContent) const;

	// Read/write rule from/to file
	static void		load(CSecureRule* pRule, QDataStream& oStream, const int nVersion);
	static void		save(const CSecureRule* pRule, QDataStream& oStream);

	// XML Import/Export functionality
	static CSecureRule*	fromXML(const QDomElement& oXMLnode);
	virtual void		toXML( QDomElement& oXMLroot ) const;

protected:
	// Contains default code for XML generation.
	static void		toXML(const CSecureRule& oRule, QDomElement& oXMLelement);
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

CSecureRule::RuleType CSecureRule::getType() const
{
	return m_nType;
}

class CIPRule : public CSecureRule
{
private:
	QHostAddress m_oIP;

public:
	CIPRule(bool bCreate = true);

	inline void			setIP( const QHostAddress& oIP );
	inline QHostAddress getIP() const;

	inline CSecureRule*	getCopy() const;

	bool				match(const QHostAddress& oAddress) const;
	void				toXML( QDomElement& oXMLroot ) const;

};

void CIPRule::setIP( const QHostAddress& oIP )
{
	m_oIP = oIP;
	m_sContent = oIP.toString();
}

QHostAddress CIPRule::getIP() const
{
	return m_oIP;
}

CSecureRule* CIPRule::getCopy() const
{
	return new CIPRule( *this );
}

class CIPRangeRule : public CSecureRule
{
private:
	QHostAddress m_oIP;
	QHostAddress m_oMask;

public:
	CIPRangeRule(bool bCreate = true);

	inline void			setIP( const QHostAddress& oIP );
	inline QHostAddress	getIP() const;
	inline void			setMask( const quint32& nMask );
	inline qint32		getMask() const;

	inline CSecureRule*	getCopy() const;

	bool				match(const QHostAddress& oAddress) const;
	void				toXML( QDomElement& oXMLroot ) const;
};

void CIPRangeRule::setIP( const QHostAddress& oIP )
{
	m_oIP = oIP;
	m_sContent = m_oIP.toString() + "/" + m_oMask.toString();
}

QHostAddress CIPRangeRule::getIP() const
{
	return m_oIP;
}

void CIPRangeRule::setMask( const quint32& nMask )
{
	m_oMask = QHostAddress( nMask );
	m_sContent = m_oIP.toString() + "/" + m_oMask.toString();
}

qint32 CIPRangeRule::getMask() const
{
	return m_oMask.toIPv4Address();
}

CSecureRule* CIPRangeRule::getCopy() const
{
	return new CIPRangeRule( *this );
}

class CCountryRule : public CSecureRule
{
public:
	CCountryRule(bool bCreate = true);

	inline CSecureRule*	getCopy() const;

	inline void			setContentString(const QString& strCountry);

	bool				match(const QHostAddress& oAddress) const;
	void				toXML( QDomElement& oXMLroot ) const;
};

CSecureRule* CCountryRule::getCopy() const
{
	return new CCountryRule( *this );
}

void CCountryRule::setContentString(const QString& strCountry)
{
	m_sContent = strCountry;
}

class CHashRule : public CSecureRule
{
private:
	QMap< CHash::Algorithm, CHash >		m_Hashes;

public:
	CHashRule(bool bCreate = true);

	inline CSecureRule*	getCopy() const;
	QList< CHash >		getHashes() const;

	void				setContent(const QList< CHash >& hashes);
	void				setContentString(const QString& sContent);
	bool				hashEquals(CHashRule& oRule) const;

	bool				match(const CFile& oFile) const;
	bool				match(const QList< CHash >& hashes) const;

	void				toXML( QDomElement& oXMLroot ) const;
};

CSecureRule* CHashRule::getCopy() const
{
	return new CHashRule( *this );
}

// There are two kinds of rules: 1. Those which contain <_>, <1>...<9> or <> 2. All other rules.
class CRegExpRule : public CSecureRule
{
private:
	bool				m_bSpecialElements;
	QRegExp				m_regExpContent;

public:
	CRegExpRule(bool bCreate = true);

	inline CSecureRule*	getCopy() const;

	void				setContentString(const QString& strContent);

	bool				match(const QList<QString>& lQuery, const QString& strContent) const;
	void				toXML( QDomElement& oXMLroot ) const;

private:
	static bool			replace(QString& sReplace, const QList<QString>& lQuery, quint8& nCurrent);
};

CSecureRule* CRegExpRule::getCopy() const
{
	return new CRegExpRule( *this );
}

class CUserAgentRule : public CSecureRule
{
private:
	bool				m_bRegExp;  // defines if the content of this rule is a regular expression filter
	QRegExp				m_regExpContent;

public:
	CUserAgentRule(bool bCreate = true);

	inline CSecureRule*	getCopy() const;

	void				setRegExp(bool bRegExp);
	inline bool			getRegExp() const;

	void				setContentString(const QString& strContent);

	bool				match(const QString& strUserAgent) const;
	void				toXML( QDomElement& oXMLroot ) const;
};

CSecureRule* CUserAgentRule::getCopy() const
{
	return new CUserAgentRule( *this );
}

bool CUserAgentRule::getRegExp() const
{
	return m_bRegExp;
}

// contains everyting that does not fit into the other rule classes
class CContentRule : public CSecureRule
{
private:
	bool				m_bAll;
	QList< QString >	m_lContent;

	typedef QList< QString >::const_iterator CListIterator;

public:
	CContentRule(bool bCreate = true);

	inline CSecureRule*	getCopy() const;

	void				setContentString(const QString& strContent);

	inline void			setAll(bool all = true);
	inline bool			getAll() const;

	bool				match(const QString& strContent) const;
	bool				match(const CFile& pFile) const;
	void				toXML( QDomElement& oXMLroot ) const;
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

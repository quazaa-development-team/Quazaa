#ifndef SECURERULE_H
#define SECURERULE_H

#include <QDomElement>
#include <QHostAddress>
#include <QString>
#include <QUuid>

#include "ShareManager/file.h"

namespace security
{

class CSecureRule
{
public:
	typedef enum { srContentUndefined, srContentAddress, srContentAddressRange, srContentCountry,
				   srContentHash, srContentAny, srContentAll, srContentRegExp, srContentUserAgent } RuleType;

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

	virtual CSecureRule*	getCopy() const;
	QString			getContentString() const;

	bool			isExpired(quint32 nNow, bool bSession = false) const;

	// Hit count control
	inline void		count() { m_nToday++; m_nTotal++; }
	inline void		resetCount() { m_nToday = m_nTotal = 0; }
	inline quint32	getTodayCount() const { return m_nToday; }
	inline quint32	getTotalCount() const { return m_nTotal; }
	inline void		loadTotalCount( quint32 nTotal ) { m_nTotal = nTotal; }

	// get the rule type
	inline RuleType	getType() const { return m_nType; }

	// Check content for hits
	virtual bool	match(const QHostAddress& oAddress) const;
	virtual bool	match(const QString& sContent) const;
	virtual bool	match(const CFile& oFile) const;
//	virtual bool	match(const CQuerySearch& oQuery, const QString& strContent) const;

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

class CIPRule : public CSecureRule
{
private:
	QHostAddress m_oIP;

public:
	CIPRule(bool bCreate = true);

	inline void			setIP( const QHostAddress& oIP ) { m_oIP = oIP; m_sContent = oIP.toString(); }
	inline QHostAddress getIP() const { return m_oIP; }

	inline CSecureRule*	getCopy() const { return new CIPRule( *this ); }

	bool				match(const QHostAddress& oAddress) const;
	void				toXML( QDomElement& oXMLroot ) const;

};

class CIPRangeRule : public CSecureRule
{
private:
	QHostAddress m_oIP;
	QHostAddress m_oMask;

public:
	CIPRangeRule(bool bCreate = true);

	inline void			setIP( const QHostAddress& oIP ) { m_oIP = oIP; m_sContent = m_oIP.toString() + "/" + m_oMask.toString(); }
	inline QHostAddress	getIP() const { return m_oIP; }
	inline void			setMask( const quint32& nMask ) { m_oMask = QHostAddress( nMask ); m_sContent = m_oIP.toString() + "/" + m_oMask.toString(); }
	inline qint32		getMask() const { return m_oMask.toIPv4Address(); }

	inline CSecureRule*	getCopy() const { return new CIPRangeRule( *this ); }

	bool				match(const QHostAddress& oAddress) const;
	void				toXML( QDomElement& oXMLroot ) const;
};

class CCountryRule : public CSecureRule
{
public:
	CCountryRule(bool bCreate = true);

	inline CSecureRule*	getCopy() const { return new CCountryRule( *this ); }

	inline void			setContentString(const QString& strCountry) { m_sContent = strCountry; }

	bool				match(const QHostAddress& oAddress) const;
	void				toXML( QDomElement& oXMLroot ) const;

};

class CHashRule : public CSecureRule
{
private:
	QMap< CHash::Algorithm, CHash >		m_Hashes;

public:
	CHashRule(bool bCreate = true);

	// Operators
	//		bool				operator==(const CHashRule& pRule);
	//		bool				operator!=(const CHashRule& pRule);

	inline CSecureRule*	getCopy() const { return new CHashRule( *this ); }
	QList< CHash >		getHashes() const;

	void				setContent(const QList< CHash >& hashes);
	void				setContentString(const QString& sContent);
	bool				hashEquals(CHashRule& oRule) const;

	bool				match(const CFile& oFile) const;
	bool				match(const QList< CHash >& hashes) const;

	void				toXML( QDomElement& oXMLroot ) const;
};

class CRegExpRule : public CSecureRule
{
public:
	CRegExpRule(bool bCreate = true);

	inline CSecureRule*	getCopy() const { return new CRegExpRule( *this ); }

	inline void			setContentString(const QString& strContent) { m_sContent = strContent; m_sContent.trimmed(); }

	//		bool				match(const CQuerySearch* pQuery, const QString& strContent) const;
	void				toXML( QDomElement& oXMLroot ) const;
};

class CUserAgentRule : public CSecureRule
{
private:
	bool				m_bRegExp;  // defines if the content of this rule is a regular expression filter

public:
	CUserAgentRule(bool bCreate = true);

	inline CSecureRule*	getCopy() const { return new CUserAgentRule( *this ); }

	inline void			setRegExp(bool bRegExp) { m_bRegExp = bRegExp; }
	inline bool			getRegExp() const { return m_bRegExp; }

	inline void			setContentString(const QString& strContent) { m_sContent = strContent; m_sContent.trimmed(); }

	bool				match(const QString& strUserAgent) const;
	void				toXML( QDomElement& oXMLroot ) const;
};

// contains everyting that does not fit into the other rule classes
class CContentRule : public CSecureRule
{
private:
	std::list< QString >	m_lContent;

	typedef std::list< QString >::const_iterator CListIterator;

public:
	CContentRule(bool bCreate = true);

	inline CSecureRule*	getCopy() const { return new CContentRule( *this ); }

	void				setContentString(const QString& strContent);

	// sets the type to "srContentAny" (false) or "srContentAll" (true).
	void				setAll(bool all = true);

	bool				match(const QString& strContent) const;
	bool				match(const CFile& pFile) const;
	void				toXML( QDomElement& oXMLroot ) const;
};

}

#endif // SECURERULE_H

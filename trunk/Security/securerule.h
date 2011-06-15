#ifndef SECURERULE_H
#define SECURERULE_H

#include <QDomElement>
#include <QHostAddress>
#include <QString>
#include <QUuid>

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
	CSecureRule&	operator=(const CSecureRule& pRule);
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
	//		virtual bool	match(const CShareazaFile& oFile) const;
	//		virtual bool	match(const CQuerySearch& oQuery, const QString& strContent) const;

	// Read/write rule from/to file
	static void		load(CSecureRule* pRule, QDataStream& oStream, const int nVersion);
	static void		save(const CSecureRule* pRule, QDataStream& oStream);

	// XML Import/Export functionality
	static CSecureRule*	fromXML(const QDomElement& oXMLnode);
	virtual void		toXML( QDomElement& oXMLroot ) const;

protected:
	// Contains default code for XML generation.
	static void		toXML(const CSecureRule& oRule, QDomElement& oXMLelement);

private:
	inline CSecureRule(const CSecureRule& oRule) { *this = oRule; }
};

class CIPRule : public CSecureRule
{
private:
	QHostAddress m_oIP;

public:
	CIPRule(bool bCreate = true);

	// Operators
	CIPRule&			operator=(const CIPRule& pRule);

	inline void			setIP( const QHostAddress& oIP ) { m_oIP = oIP; m_sContent = oIP.toString(); }
	inline QHostAddress getIP() const { return m_oIP; }

	inline CSecureRule*	getCopy() const { return new CIPRule( *this ); }

	bool				match(const QHostAddress& oAddress) const;
	void				toXML( QDomElement& oXMLroot ) const;

private:
	inline CIPRule(const CIPRule& pRule) { *this = pRule; }
};

class CIPRangeRule : public CSecureRule
{
private:
	QHostAddress m_oIP;
	QHostAddress m_oMask;

public:
	CIPRangeRule(bool bCreate = true);

	// Operators
	CIPRangeRule&		operator=(const CIPRangeRule& pRule);

	inline void			setIP( const QHostAddress& oIP ) { m_oIP = oIP; m_sContent = m_oIP.toString() + "/" + m_oMask.toString(); }
	inline QHostAddress	getIP() const { return m_oIP; }
	inline void			setMask( const quint32& nMask ) { m_oMask = QHostAddress( nMask ); m_sContent = m_oIP.toString() + "/" + m_oMask.toString(); }
	inline qint32		getMask() const { return m_oMask.toIPv4Address(); }

	inline CSecureRule*	getCopy() const { return new CIPRangeRule( *this ); }

	bool				match(const QHostAddress& oAddress) const;
	void				toXML( QDomElement& oXMLroot ) const;
private:
	inline CIPRangeRule(const CIPRangeRule& oRule) { *this = oRule; }
};

class CCountryRule : public CSecureRule
{
public:
	CCountryRule(bool bCreate = true);

	// Operator
	CCountryRule&		operator=(const CCountryRule& pRule);

	inline CSecureRule*	getCopy() const { return new CCountryRule( *this ); }

	inline void			setContentString(const QString& strCountry) { m_sContent = strCountry; }

	bool				match(const QHostAddress& oAddress) const;
	void				toXML( QDomElement& oXMLroot ) const;
private:
	inline				CCountryRule(const CCountryRule& oRule) { *this = oRule; }
};

class CHashRule : public CSecureRule
{
private:
	QString				m_sSHA1;	// SHA1 (Base32)
	QString				m_sED2K;	// ED2K (MD4, Base16)
	QString				m_sBTIH;	// BitTorrent Info Hash (Base32)
	QString				m_sTTH;		// TigerTree Root Hash (Base32)
	QString				m_sMD5;		// MD5 (Base16)

	// Allows to compare 2 rules by the importance of the hashes they contain and allows to
	// check whether 2 rules are using exactly the same hashes (uses prime number multiplication).
	unsigned int		m_nHashWeight;

public:
	CHashRule(bool bCreate = true);

	// Operators
	CHashRule&			operator=(const CHashRule& pRule);
	//		bool				operator==(const CHashRule& pRule);
	//		bool				operator!=(const CHashRule& pRule);

	inline CSecureRule*	getCopy() const { return new CHashRule( *this ); }

	void				setContentString(const QString& strContent);

	inline QString		getSHA1urn() const { return m_sSHA1; }
	inline QString		getED2Kurn() const { return m_sED2K; }
	inline QString		getBTIHurn() const { return m_sBTIH; }
	inline QString		getTTHurn()  const { return m_sTTH;  }
	inline QString		getMD5urn()  const { return m_sMD5;  }

	inline unsigned int	getHashWeight() const { return m_nHashWeight; }

	//		bool				match(const CShareazaFile* pFile) const;
	bool				match(const QString& sSHA1,
							  const QString& sED2K,
							  const QString& sTTH  = "",
							  const QString& sBTIH = "",
							  const QString& sMD5  = "") const;

	void				toXML( QDomElement& oXMLroot ) const;

private:
	inline CHashRule(const CHashRule& pRule) { *this = pRule; }
};

class CRegExpRule : public CSecureRule
{
public:
	CRegExpRule(bool bCreate = true);

	// Operators
	CRegExpRule&		operator=(const CRegExpRule& pRule);

	inline CSecureRule*	getCopy() const { return new CRegExpRule( *this ); }

	inline void			setContentString(const QString& strContent) { m_sContent = strContent; m_sContent.trimmed(); }

	//		bool				match(const CQuerySearch* pQuery, const QString& strContent) const;
	void				toXML( QDomElement& oXMLroot ) const;

private:
	inline CRegExpRule(const CRegExpRule& pRule) { *this = pRule; }
};

class CUserAgentRule : public CSecureRule
{
private:
	bool				m_bRegExp;  // defines if the content of this rule is a regular expression filter

public:
	CUserAgentRule(bool bCreate = true);

	// Operators
	CUserAgentRule&		operator=(const CUserAgentRule& pRule);

	inline CSecureRule*	getCopy() const { return new CUserAgentRule( *this ); }

	inline void			setRegExp(bool bRegExp) { m_bRegExp = bRegExp; }
	inline bool			getRegExp() const { return m_bRegExp; }

	inline void			setContentString(const QString& strContent) { m_sContent = strContent; m_sContent.trimmed(); }

	bool				match(const QString& strUserAgent) const;
	void				toXML( QDomElement& oXMLroot ) const;

private:
	inline CUserAgentRule(const CUserAgentRule& pRule) { *this = pRule; }
};

// contains everyting that does not fit into the other rule classes
class CContentRule : public CSecureRule
{
private:
	std::list< QString >	m_lContent;

	typedef std::list< QString >::const_iterator CListIterator;

public:
	CContentRule(bool bCreate = true);

	// Operators
	CContentRule&		operator=(const CContentRule& pRule);

	inline CSecureRule*	getCopy() const { return new CContentRule( *this ); }

	void				setContentString(const QString& strContent);

	// sets the type to "srContentAny" (false) or "srContentAll" (true).
	void				setAll(bool all = true);

	bool				match(const QString& strContent) const;
	//		bool				match(const CShareazaFile* pFile) const;
	void				toXML( QDomElement& oXMLroot ) const;

private:
	inline CContentRule(const CContentRule& pRule) { *this = pRule; }
};

}

#endif // SECURERULE_H

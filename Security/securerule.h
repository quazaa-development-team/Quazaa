#ifndef SECURERULE_H
#define SECURERULE_H

#include <QHostAddress>
#include <QString>
#include <QUuid>

#include "security.h"

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

	private:
		// Hit counters
		quint32		m_nToday;
		quint32		m_nTotal;

	public:
		Policy		m_nAction;
		QUuid		m_pUUID;
		quint32		m_nExpire;
		QString		m_sComment;

	public:
		// Construction / Destruction
		CSecureRule(bool bCreate = true);
		virtual inline	~CSecureRule() {}

		// Operators
		CSecureRule&	operator=(const CSecureRule& pRule);

		bool			IsExpired(quint32 nNow, bool bSession = false) const;
		void			Remove();

		virtual CSecureRule*	GetCopy() const;

		// Hit count control
		inline void		Count() { m_nToday++; m_nTotal++; }
		inline void		Reset() { m_nToday = m_nTotal = 0; }
		inline quint32	TodayCount() const { return m_nToday; }
		inline quint32	TotalCount() const { return m_nTotal; }
		inline void		TotalCount( quint32 nTotal ) { m_nTotal = nTotal; }

		// Get the rule type
		inline RuleType Type() const { return m_nType; }

		// Check content for hits
		virtual bool	Match(const QHostAddress* pAddress) const;
		virtual bool	Match(const QString& strContent) const;
//		virtual bool	Match(const CShareazaFile* pFile) const;
//		virtual bool	Match(const CQuerySearch* pQuery, const QString& strContent) const;

		// Export functionality
//		virtual QDomDocument*	ToXML() const;

		// Import functionality
//		static CSecureRule*		FromXML(CXMLElement* pXML);

		// Read/write rule to archive
//		static void		Serialize(CSecureRule* &pSecureRule, CArchive& ar, int nVersion);

	protected:
		// Contains default code for XML generation.
//		static void		ToXML(const CSecureRule* const &rule, CXMLElement* &pXML);

	private:
		inline CSecureRule(const CSecureRule& pRule) { *this = pRule; }
	};

	class CIPRule : public CSecureRule
	{
	private:
		QHostAddress m_oIP;

	public:
		CIPRule(bool bCreate = true);
		inline ~CIPRule() {}

		// Operators
		CIPRule&	operator=(const CIPRule& pRule);

		inline CSecureRule*	GetCopy() const { return new CIPRule( *this ); }

		bool			Match(const QHostAddress* pAddress) const;

//		CXMLElement*	ToXML() const;

	private:
		inline CIPRule(const CIPRule& pRule) { *this = pRule; }
	};

	class CIPRangeRule : public CSecureRule
	{
	private:
		QHostAddress m_oIP;
		qint32       m_nMask;
//		BYTE		m_nMask[4];

	public:
		CIPRangeRule(bool bCreate = true);
		inline ~CIPRangeRule() {}

		// Operators
		CIPRangeRule& operator=(const CIPRangeRule& pRule);

		inline CSecureRule*	GetCopy() const { return new CIPRangeRule( *this ); }

		bool		Match(const QHostAddress* pAddress) const;

//		CXMLElement*	ToXML() const;
	private:
		inline CIPRangeRule(const CIPRangeRule& pRule) { *this = pRule; }
	};

	class CCountryRule : public CSecureRule
	{
	private:
		QString		m_sCountry;

	public:
		CCountryRule(bool bCreate = true);
		inline ~CCountryRule() {}

		// Operator
		CCountryRule&	operator=(const CCountryRule& pRule);
		inline CSecureRule*	GetCopy() const { return new CCountryRule( *this ); }

		inline void		SetContent(const QString& strCountry) { m_sCountry = strCountry; }
		inline QString	GetContent() const { return m_sCountry; }

		bool			Match(const QHostAddress* pAddress) const;

//		CXMLElement*	ToXML() const;
	private:
		inline CCountryRule(const CCountryRule& pRule) { *this = pRule; }
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
		unsigned int	m_nHashWeight;

	public:
		CHashRule(bool bCreate = true);
		inline ~CHashRule() {}

		// Operators
		CHashRule& operator=(const CHashRule& pRule);
		bool operator==(const CHashRule& pRule);
		bool operator!=(const CHashRule& pRule);

		inline CSecureRule*	GetCopy() const { return new CHashRule( *this ); }

		void		SetContentWords(const QString& strContent);
		QString		GetContentWords() const;

		inline QString		GetSHA1urn() const { return m_sSHA1; }
		inline QString		GetED2Kurn() const { return m_sED2K; }
		inline QString		GetBTIHurn() const { return m_sBTIH; }
		inline QString		GetTTHurn()  const { return m_sTTH;  }
		inline QString		GetMD5urn()  const { return m_sMD5;  }

		inline unsigned int	GetHashWeight() const { return m_nHashWeight; }

//		bool		Match(const CShareazaFile* pFile) const;
		bool		Match(const QString& sSHA1,
						  const QString& sED2K,
						  const QString& sTTH  = "",
						  const QString& sBTIH = "",
						  const QString& sMD5  = "") const;

//		CXMLElement*	ToXML() const;

	private:
		inline CHashRule(const CHashRule& pRule) { *this = pRule; }
	};

	class CRegExpRule : public CSecureRule
	{
	private:
		QString		m_sContent;

	public:
		CRegExpRule(bool bCreate = true);
		inline ~CRegExpRule() {}

		// Operators
		CRegExpRule&	operator=(const CRegExpRule& pRule);

		inline CSecureRule*	GetCopy() const { return new CRegExpRule( *this ); }

		void		SetContentWords(const QString& strContent);
		QString		GetContentWords() const;

//		bool		Match(const CQuerySearch* pQuery, const QString& strContent) const;

//		CXMLElement*	ToXML() const;

	private:
		inline CRegExpRule(const CRegExpRule& pRule) { *this = pRule; }
	};

	class CUserAgentRule : public CSecureRule
	{
	private:
		QString		m_sContent;
		bool		m_bRegExp;  // defines if the content of this rule is a regular expression filter

	public:
		CUserAgentRule(bool bCreate = true);
		inline ~CUserAgentRule() {}

		// Operators
		CUserAgentRule& operator=(const CUserAgentRule& pRule);

		inline CSecureRule*	GetCopy() const { return new CUserAgentRule( *this ); }

		void		SetContentWords(const QString& strContent);
		QString		GetContentWords() const;

		bool		Match(const QString& strUserAgent) const;

//		CXMLElement*	ToXML() const;

	private:
		inline CUserAgentRule(const CUserAgentRule& pRule) { *this = pRule; }
	};

	// contains everyting that does not fit into the other rule classes
	class CContentRule : public CSecureRule
	{
	private:
		QString		m_sContent;

	public:
		CContentRule(bool bCreate = true);
		inline ~CContentRule() {}

		// Operators
		CContentRule&	operator=(const CContentRule& pRule);

		inline CSecureRule*	GetCopy() const { return new CContentRule( *this ); }

		void		SetContentWords(const QString& strContent);
		QString		GetContentWords() const;

		// Sets the type to "srContentAny" (false) or "srContentAll" (true).
		void		SetAll(bool all = true);

		bool		Match(const QString& strContent) const;
//		bool		Match(const CShareazaFile* pFile) const;

//		CXMLElement*	ToXML() const;

	private:
		inline CContentRule(const CContentRule& pRule) { *this = pRule; }
	};

};

#endif // SECURERULE_H

#ifndef REGEXPRULE_H
#define REGEXPRULE_H

#include "securerule.h"

class CRegExpRule : public CSecureRule
{
private:
	// There are two kinds of rules:
	// 1. Those which contain <_>, <1>...<9> or <> (e.g. special elements)
	// 2. All other rules.
	bool				m_bSpecialElements; // contains special elements

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

#endif // REGEXPRULE_H

#ifndef REGEXPRULE_H
#define REGEXPRULE_H

#include "securerule.h"

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

#endif // REGEXPRULE_H

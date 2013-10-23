#ifndef CONTENTRULE_H
#define CONTENTRULE_H

#include "securerule.h"

class CContentRule : public CSecureRule
{
private:
	bool				m_bAll;
	QList< QString >	m_lContent;

	typedef QList< QString >::const_iterator CListIterator;

public:
	CContentRule();

	bool				operator==(const CSecureRule& pRule) const;

	void			setAll(bool all = true);
	bool			getAll() const;

	bool				parseContent(const QString& sContent);

	CSecureRule*	getCopy() const;

	bool				match(const QString& sFileName) const;
	bool				match(const CQueryHit* const pHit) const;

	void				toXML(QXmlStreamWriter& oXMLdocument) const;
};

#endif // CONTENTRULE_H

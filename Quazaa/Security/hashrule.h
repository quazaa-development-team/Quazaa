#ifndef HASHRULE_H
#define HASHRULE_H

#include "securerule.h"

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

	bool				hashEquals(CHashRule *oRule) const;

	bool				match(const CQueryHit* const pHit) const;
	bool				match(const QList<CHash>& lHashes) const;

	void				toXML(QXmlStreamWriter& oXMLdocument) const;
};

#endif // HASHRULE_H

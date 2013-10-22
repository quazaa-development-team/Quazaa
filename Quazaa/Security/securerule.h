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

#include "NetworkCore/endpoint.h"
#include "NetworkCore/Hashes/hash.h"
#include "NetworkCore/queryhit.h"

// Note: The locking information within the doxygen comments refers to the RW lock of the Security
//       Manager.

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

	bool     isExpired(quint32 tNow, bool bSession = false) const;
	quint32  getExpiryTime() const;

	// Hit count control
	void     count();
	void     resetCount();
	quint32  getTodayCount() const;
	quint32  getTotalCount() const;
	void     loadTotalCount(quint32 nTotal);

	// get the rule type
	TRuleType type() const;

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
	virtual void toXML(QXmlStreamWriter& oXMLdocument) const;

protected:
	// Contains default code for XML generation.
	static void         toXML(const CSecureRule& oRule, QXmlStreamWriter& oXMLdocument);
};

#endif // SECURERULE_H

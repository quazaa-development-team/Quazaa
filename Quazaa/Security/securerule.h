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

#include <QRegularExpression>

#include <QString>
#include <QUuid>
#include <QAtomicInt>

#include "NetworkCore/endpoint.h"
#include "NetworkCore/Hashes/hash.h"
#include "NetworkCore/queryhit.h"

// Note: The locking information within the doxygen comments refers to the RW lock of the Security
//       Manager.

typedef quint16 TRuleID; // used for GUI updating

namespace RuleType {
	enum Type { Undefined = 0, IPAddress = 1, IPAddressRange = 2, Hash = 4, RegularExpression = 5, UserAgent = 6, Content = 7 };
}

namespace RuleAction {
	enum Action { None = 0, Accept = 1, Deny = 2 };
}

namespace RuleTime {
	enum Time	{
		Special = -1, FiveMinutes = 300, ThirtyMinutes = 1800, TwoHours = 7200, SixHours = 21600,
		TwelveHours = 42300, Day = 86400, Week = 604800, Month = 2592000
	};
}

class CSecureRule : public QObject
{
	Q_OBJECT

protected:
	// Type is critical to functionality and may not be changed externally.
	RuleType::Type	m_nType;

	// Contains a string representation of the rule content for faster GUI accesses.
	// Can be accessed via getContentString().
	QString			m_sContent;

private:
	// Hit counters
	QAtomicInt	m_nToday;
	QAtomicInt	m_nTotal;
	quint32		m_tExpire;
	bool		m_bForever;
	bool		m_bModifyLock;
	bool		m_bRemoving;

public:
	RuleAction::Action	m_nAction;
	QUuid				m_oUUID;
	QString				m_sComment;
	bool				m_bAutomatic;

public:
	// Construction / Destruction
	CSecureRule();
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

	bool	isExpired(quint32 tNow, bool bSession = false) const;
	void	setExpiryTime(const quint32& time);
	quint32	getExpiryTime() const;
	void	setForever(bool bForever);
	bool	isForever();

	// Modify locking to avoid deleting pointers to files being modified.
	void	setLockForModify(bool lock);
	bool	isLockedForModify();
	void	beingRemoved(bool remove);
	bool	isBeingRemoved();

	// Hit count control
	void     count();
	void     resetCount();
	quint32  getTodayCount() const;
	quint32  getTotalCount() const;
	void     loadTotalCount(quint32 nTotal);

	// get the rule type
	RuleType::Type type() const;

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

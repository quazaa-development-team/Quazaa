#ifndef SECURITYFILTERMODELAUTOMATIC_H
#define SECURITYFILTERMODELAUTOMATIC_H

#include <QSortFilterProxyModel>
#include "securitytablemodel.h"

class SecurityFilterModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	SecurityFilterModel(CSecurityTableModel* target, bool bShowAutomatic);

	void sort(int column, Qt::SortOrder order);

protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
	CSecurityTableModel* m_lSecurity;
	bool m_bShowAutomatic;

};

#endif // SECURITYFILTERMODELAUTOMATIC_H

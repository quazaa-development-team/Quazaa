#ifndef SECURITYTABLEMODEL_H
#define SECURITYTABLEMODEL_H

#include <QAbstractItemView>
#include <QAbstractTableModel>
#include <QVector>
#include <QIcon>

#include "Security/security.h"

class CSecurityTableModel : public QAbstractTableModel
{
    Q_OBJECT

private:
	typedef security::CSecureRule CSecureRule;

	QWidget*		m_oContainer;
	int				m_nSortColumn;
	Qt::SortOrder	m_nSortOrder;
	bool			m_bNeedSorting;

public:
	enum Column
	{
		CONTENT = 0,
		ACTION,
		EXPIRES,
		HITS,
		COMMENT,
		_NO_OF_COLUMNS
	};

	struct Rule
	{
		CSecureRule*		pNode;

		QString				sContent;
		CSecureRule::Policy	nAction;
		quint32				tExpire;
		quint32				nToday;
		quint32				nTotal;
		QString				sComment;
		QIcon				iAction;

		Rule(const CSecureRule* const pNode);
		bool update(int row, int col, QModelIndexList& to_update, CSecurityTableModel* model);
		QVariant data(int col) const;
		bool lessThan(int col, const CSecurityTableModel::Rule* const pOther) const;

		QString actionToString(CSecureRule::Policy nAction) const;
		QString expiryToString(quint32 tExpire) const;
	};

protected:
	QVector<Rule*>   m_lNodes;

public:
	explicit CSecurityTableModel(QObject* parent = 0, QWidget* container = 0);
	~CSecurityTableModel();

	int rowCount(const QModelIndex& parent) const;
	int columnCount(const QModelIndex& parent) const;
	QVariant data(const QModelIndex& index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

	void sort(int column, Qt::SortOrder order);

	CSecureRule* nodeFromIndex(const QModelIndex& index);

public slots:
	void addRule(const QSharedPointer<CSecureRule> pRule);
	void removeRule(const QSharedPointer<CSecureRule> pRule);
	void updateAll();
};

#endif // SECURITYTABLEMODEL_H

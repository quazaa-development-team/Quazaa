//
// neighbourstablemodel.h
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef NEIGHBOURSTABLEMODELL_H
#define NEIGHBOURSTABLEMODELL_H

#include <QAbstractTableModel>
#include "types.h"
#include <QList>
#include <QTime>
#include <QMutex>
#include <QIcon>

class CG2Node;

typedef struct
{
	CG2Node*	  pNode;
	IPv4_ENDPOINT oAddress;
	G2NodeType    nType;
	int           nState;
	quint32       tConnected;
	quint32       nPacketsIn;
	quint32       nPacketsOut;
	quint32       nBandwidthIn;
	quint32       nBandwidthOut;
	quint64       nBytesSent;
	quint64       nBytesReceived;
	float         nCompressionIn;
	float         nCompressionOut;
	quint32       nLeafCount;
	quint32       nLeafMax;
	qint64        nRTT;
	QString       sUserAgent;
	QString		  sCountry;
	QIcon		  iNetwork;
	QIcon		  iCountry;
} sNeighbour;

class CNeighboursTableModel : public QAbstractTableModel
{
	Q_OBJECT

protected:
	QList<sNeighbour>   m_lNodes;
public:
	explicit CNeighboursTableModel(QObject *parent = 0, QWidget *container = 0);
	~CNeighboursTableModel();

	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	//Qt::ItemFlags flags(const QModelIndex &index) const;
	QList<sNeighbour>* GetList()
	{
		return &m_lNodes;
	}


protected:
	QString StateToString(int s) const;
	QString TypeToString(G2NodeType t) const;

private:
	QWidget *m_oContainer;

public slots:
	bool NodeExists(CG2Node* pNode);
	void OnNodeAdded(CG2Node* pNode);
	void AddNode(CG2Node* pNode, bool bSignal = true);
	void UpdateNeighbourData(CG2Node* pNode);
	void UpdateNode(CG2Node* pNode, bool bSignal = true);
	void OnRemoveNode(CG2Node* pNode);
	void UpdateAll();
};

#endif // NEIGHBOURSTABLEMODELL_H

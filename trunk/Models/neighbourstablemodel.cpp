//
// neighbourstablemodel.cpp
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

#include "neighbourstablemodel.h"

#include <QColor>
#include <QSize>
#include <QIcon>
#include <QMutexLocker>
#include <QFont>
#include "g2node.h"
#include "network.h"
#include "neighbours.h"
#include "geoiplist.h"
#include "QSkinDialog/qskinsettings.h"

CNeighboursTableModel::CNeighboursTableModel(QObject *parent, QWidget *container) :
	QAbstractTableModel(parent)
{
	m_oContainer = container;

	connect(&Network, SIGNAL(NodeAdded(CG2Node*)), this, SLOT(OnNodeAdded(CG2Node*)), Qt::QueuedConnection);
	connect(&Network, SIGNAL(NodeUpdated(CG2Node*)), this, SLOT(UpdateNeighbourData(CG2Node*)), Qt::QueuedConnection);
	connect(&Network, SIGNAL(NodeRemoved(CG2Node*)), this, SLOT(OnRemoveNode(CG2Node*)), Qt::QueuedConnection);
}

CNeighboursTableModel::~CNeighboursTableModel()
{
	m_lNodes.clear();
}

int CNeighboursTableModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	if( parent.isValid() )
		return 0;

	return m_lNodes.count();
}

int CNeighboursTableModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return 11;
}

QVariant CNeighboursTableModel::data(const QModelIndex& index, int role) const
{
	if( !index.isValid() )
		return QVariant();

	if( index.row() > m_lNodes.size() || index.row() < 0 )
		return QVariant();

	if ( role == Qt::DecorationRole )
	{
		const sNeighbour& n = m_lNodes.at(index.row());
		if (index.column() == 0)
		{
			return n.iNetwork;
		}

		if(index.column() == 10)
		{
			return n.iCountry;
		}
	}

	if( role == Qt::DisplayRole )
	{
		const sNeighbour& n = m_lNodes.at(index.row());
		switch( index.column() )
		{
		case 0:
			return n.pNode->GetAddress().toString();
		case 1:
			return StateToString(n.nState);
		case 2:
			return QString().sprintf("%.2u:%.2u:%.2u", n.tConnected / 3600, (n.tConnected % 3600 / 60), (n.tConnected % 3600) % 60);
		case 3:
			return QString().sprintf("%1.3f / %1.3f", n.nBandwidthIn / 1024.0f, n.nBandwidthOut / 1024.0f);
		case 4:
			return QString().sprintf("%1.3f / %1.3f KB (%1.2f / %1.2f %%)", n.nBytesReceived / 1024.0f, n.nBytesSent / 1024.0f, 100.0f * n.nBytesReceived / n.nCompressionIn, 100.0f * n.nBytesSent / n.nCompressionOut );
		case 5:
			return QString().sprintf("%u - %u", n.nPacketsIn, n.nPacketsOut);
		case 6:
			return TypeToString(n.nType);
		case 7:
			if( n.nType == G2_HUB )
			{
				return QString().sprintf("%u / %u", n.nLeafCount, n.nLeafMax);
			}
			else
				return QString();
		case 8:
			return QString("%1 msec").arg(n.nRTT);
		case 9:
			return n.sUserAgent;
		case 10:
			return n.sCountry;
		}
	}

	else if( role == Qt::ForegroundRole )
	{
		switch ( m_lNodes.at(index.row()).nState )
		{
		case nsConnected:
			return skinSettings.listsColorSpecial;
			break;
		case nsConnecting:
			return skinSettings.listsColorActive;
			break;
		default:
			return skinSettings.listsColorNormal;
			break;
		}
	}

	else if( role == Qt::FontRole )
	{
		QFont font = qApp->font(m_oContainer);
		switch ( m_lNodes.at(index.row()).nState )
		{
		case nsConnected:
			font.setWeight(skinSettings.listsWeightSpecial);
			return font;
			break;
		case nsConnecting:
			font.setWeight(skinSettings.listsWeightActive);
			return font;
			break;
		default:
			font.setWeight(skinSettings.listsWeightNormal);
			return font;
			break;
		}
	}

	return QVariant();
}

QVariant CNeighboursTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if( role == Qt::DisplayRole && orientation == Qt::Horizontal )
	{
		switch( section )
		{
		case 0:
			   return "Address";
		case 1:
			   return "Status";
		case 2:
			   return "Time";
		case 3:
			   return "Bandwidth";
		case 4:
			   return "Bytes";
		case 5:
			   return "Packets";
		case 6:
			   return "Mode";
		case 7:
			   return "Leaves";
		case 8:
			   return "Ping";
		case 9:
			   return "User Agent";
		case 10:
			   return "Country";
		}
	}

	/*else if( role == Qt::SizeHintRole && orientation == Qt::Horizontal )
	{
		switch(section)
		{
		case 0:
			return QSize(200, 20);
		case 1:
			return QSize(100, 20);
		}
	}*/

	return QVariant();
}

bool CNeighboursTableModel::NodeExists(CG2Node *pNode)
{
	if( Neighbours.GetCount() == 0 )
		return false;

	for( QList<CG2Node*>::iterator itNode = Neighbours.begin(); itNode != Neighbours.end(); ++itNode )
	{
		if( pNode == *itNode )
			return true;
	}

	return false;
}

void CNeighboursTableModel::OnNodeAdded(CG2Node* pNode)
{
	Q_UNUSED(pNode);

	/*qDebug() << "OnNodeAdded";

	if( !Network.m_pSection.tryLock(50) ) // if can't get lock - forget it
		return;

	if( NodeExists(pNode) )
		AddNode(pNode, false);

	Network.m_pSection.unlock();*/
}

void CNeighboursTableModel::AddNode(CG2Node *pNode, bool bSignal)
{
	//qDebug() << "AddNode";

	sNeighbour nbr;

	quint32 tNow = time(0);

	beginInsertRows(QModelIndex(), m_lNodes.size(), m_lNodes.size());
	nbr.pNode = pNode;
	nbr.iNetwork = QIcon(":/Resource/Networks/Gnutella2.png");
	nbr.tConnected = tNow - pNode->m_tConnected;
	nbr.nBandwidthIn = pNode->m_mInput.Usage();
	nbr.nBandwidthOut = pNode->m_mOutput.Usage();
	nbr.nBytesReceived = pNode->m_mInput.m_nTotal;
	nbr.nBytesSent = pNode->m_mOutput.m_nTotal;
	nbr.nCompressionIn = pNode->GetTotalInDecompressed();
	nbr.nCompressionOut = pNode->GetTotalOutCompressed();
	nbr.nLeafCount = pNode->m_nLeafCount;
	nbr.nLeafMax = pNode->m_nLeafMax;
	nbr.nPacketsIn = pNode->m_nPacketsIn;
	nbr.nPacketsOut = pNode->m_nPacketsOut;
	nbr.nRTT = pNode->m_tRTT;
	nbr.nState = pNode->m_nState;
	nbr.nType = pNode->m_nType;
	nbr.sUserAgent = pNode->m_sUserAgent;
	QString sCountry = GeoIP.findCountryCode(nbr.pNode->m_oAddress);
	nbr.sCountry = GeoIP.countryNameFromCode(sCountry);
	nbr.iCountry = QIcon(":/Resource/Flags/" + sCountry.toLower() + ".png");


	m_lNodes.append(nbr);
	endInsertRows();

	if( bSignal )
	{
		QModelIndex idxUpdate = index(m_lNodes.size() - 1, 0, QModelIndex());
		QModelIndex idxUpdate2 = index(m_lNodes.size() - 1, 10, QModelIndex());
		emit dataChanged(idxUpdate, idxUpdate2);
	}
}

void CNeighboursTableModel::UpdateNeighbourData(CG2Node* pNode)
{
	//qDebug() << "UpdateNeighboursData";

	if( !Neighbours.m_pSection.tryLock() )
		return;	// forget it

	if( NodeExists(pNode) )
		UpdateNode(pNode);

	Neighbours.m_pSection.unlock();
}
void CNeighboursTableModel::UpdateNode(CG2Node *pNode, bool bSignal)
{
	//qDebug() << "UpdateNode";

	quint32 tNow = time(0);

	for( int i = 0; i < m_lNodes.size(); i++)
	{
		if( m_lNodes.at(i).pNode == pNode )
		{
			m_lNodes[i].tConnected = tNow - pNode->m_tConnected;
			m_lNodes[i].nBandwidthIn = pNode->m_mInput.Usage();
			m_lNodes[i].nBandwidthOut = pNode->m_mOutput.Usage();
			m_lNodes[i].nBytesReceived = pNode->m_mInput.m_nTotal;
			m_lNodes[i].nBytesSent = pNode->m_mOutput.m_nTotal;
			m_lNodes[i].nCompressionIn = pNode->GetTotalInDecompressed();
			m_lNodes[i].nCompressionOut = pNode->GetTotalOutCompressed();
			m_lNodes[i].nLeafCount = pNode->m_nLeafCount;
			m_lNodes[i].nLeafMax = pNode->m_nLeafMax;
			m_lNodes[i].nPacketsIn = pNode->m_nPacketsIn;
			m_lNodes[i].nPacketsOut = pNode->m_nPacketsOut;
			m_lNodes[i].nRTT = pNode->m_tRTT;
			m_lNodes[i].nState = pNode->m_nState;
			m_lNodes[i].nType = pNode->m_nType;
			m_lNodes[i].sUserAgent = pNode->m_sUserAgent;

			if( bSignal )
			{
				QModelIndex idx = index(i, 0, QModelIndex());
				QModelIndex idx2 = index(i, 10, QModelIndex());
				emit dataChanged(idx, idx2);
			}

			break;
		}
	}
}

void CNeighboursTableModel::OnRemoveNode(CG2Node* pNode)
{
	//qDebug() << "OnRemoveNode";

	for( int i = 0, nMax = m_lNodes.size(); i < nMax; i++ )
	{
		if( m_lNodes[i].pNode == pNode )
		{
			beginRemoveRows(QModelIndex(), i, i);
			m_lNodes.removeAt(i);
			endRemoveRows();
			/*if( sender() )
			{
				QModelIndex idx1 = index(i, 0, QModelIndex());
				QModelIndex idx2 = index(i, 10, QModelIndex());
				emit dataChanged(idx1, idx2);
			}*/
			break;
		}
	}
}

QString CNeighboursTableModel::StateToString(int s) const
{
	switch(s)
	{
	case nsClosed:
		return "closed";
	case nsConnecting:
		return "connecting";
	case nsHandshaking:
		return "handshaking";
	case nsConnected:
		return "connected";
	case nsClosing:
		return "closing";
	case nsError:
		return "error";
	}

	return QString();
}
QString CNeighboursTableModel::TypeToString(G2NodeType t) const
{
	if( t == G2_HUB )
		return "G2 Hub";
	else if( t == G2_LEAF )
		return "G2 Leaf";
	else
		return "";
}

void CNeighboursTableModel::UpdateAll()
{

	//qDebug() << "UpdateAll";

	if( Neighbours.m_pSection.tryLock() )
	{
		// first check if we need to remove some nodes

		for( int i = 0; i < m_lNodes.size(); i++ )
		{
			bool bFound = Neighbours.NeighbourExists(m_lNodes.at(i).pNode);

			if( !bFound )
			{
				// not found in network - remove it
				OnRemoveNode(m_lNodes.at(i).pNode);
			}
		}

		// now add missing nodes or update existing

		for( QList<CG2Node*>::iterator itNode = Neighbours.begin(); itNode != Neighbours.end(); ++itNode )
		{
			bool bFound = false;

			for( int j = 0; j < m_lNodes.size(); j++ )
			{
				if( *itNode == m_lNodes.at(j).pNode )
				{
					UpdateNode(*itNode, false);
					bFound = true;
					break;
				}
			}

			if( !bFound )
			{
				AddNode(*itNode, false);
			}
		}

		Neighbours.m_pSection.unlock();

		QModelIndex idx1 = index(0, 0, QModelIndex());
		QModelIndex idx2 = index(m_lNodes.size() - 1, 10, QModelIndex());
		emit dataChanged(idx1, idx2);
	}
}

#include "NeighboursTableModel.h"

#include <QColor>
#include <QSize>
#include <QIcon>
#include <QMutexLocker>
#include "g2node.h"
#include "network.h"
#include "geoiplist.h"

CNeighboursTableModel::CNeighboursTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
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

    return m_lNodes.size();
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
		if (index.column() == 0)
		{
			QIcon icon(":/Resource/Networks/Gnutella2.png");
			return icon;
		}

		/*if(index.column() == 10)
		{
			sNeighbour n = m_lNodes.at(index.row());
			QIcon icon(":/Resource/Flags/" + GeoIP.findCountryCode(n.pNode->GetAddress().toString()).toLower() + ".png");
			return icon;
		}*/
	}

    if( role == Qt::DisplayRole )
    {
        sNeighbour n = m_lNodes.at(index.row());
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
            return QString().sprintf("%u msec", n.nRTT);
        case 9:
            return n.sUserAgent;
		case 10:
			return n.sCountry;
        }
    }
    else if( role == Qt::ForegroundRole )
    {
        if( m_lNodes.at(index.row()).nState == nsConnected )
            return QColor(0, 0, 180);
        else
            return QColor(150, 150, 150);
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

    else if( role == Qt::SizeHintRole && orientation == Qt::Horizontal )
    {
        switch(section)
        {
        case 0:
			return QSize(200, 20);
        case 1:
			return QSize(100, 20);
        }
    }

    return QVariant();
}
bool CNeighboursTableModel::NodeExists(CG2Node *pNode)
{
	if( Network.m_lNodes.isEmpty() )
		return false;

	for( int i = 0; i < Network.m_lNodes.size(); i++ )
	{
		if( Network.m_lNodes.at(i) == pNode )
			return true;
	}

	return false;
}

void CNeighboursTableModel::OnNodeAdded(CG2Node* pNode)
{
	/*qDebug() << "OnNodeAdded";

	if( !Network.m_pSection.tryLock(50) ) // if can't get lock - forget it
		return;

	if( NodeExists(pNode) )
		AddNode(pNode, false);

	Network.m_pSection.unlock();*/
}

void CNeighboursTableModel::AddNode(CG2Node *pNode, bool bSignal)
{
	qDebug() << "AddNode";

	sNeighbour nbr;

	quint32 tNow = time(0);

	beginInsertRows(QModelIndex(), m_lNodes.size(), m_lNodes.size());
	nbr.pNode = pNode;
	nbr.tConnected = tNow - pNode->m_tConnected;
	nbr.nBandwidthIn = pNode->AvgIn(tNow);
	nbr.nBandwidthOut = pNode->AvgOut(tNow);
	nbr.nBytesReceived = pNode->GetTotalIn();
	nbr.nBytesSent = pNode->GetTotalOut();
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
	nbr.sCountry = "";

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
	qDebug() << "UpdateNeighboursData";

	if( !Network.m_pSection.tryLock(50) )
		return;	// forget it

	if( NodeExists(pNode) )
		UpdateNode(pNode);

	Network.m_pSection.unlock();
}
void CNeighboursTableModel::UpdateNode(CG2Node *pNode, bool bSignal)
{
	qDebug() << "UpdateNode";

	quint32 tNow = time(0);

	for( int i = 0; i < m_lNodes.size(); i++)
	{
		if( m_lNodes[i].pNode == pNode )
		{
			m_lNodes[i].tConnected = tNow - pNode->m_tConnected;
			m_lNodes[i].nBandwidthIn = pNode->AvgIn(tNow);
			m_lNodes[i].nBandwidthOut = pNode->AvgOut(tNow);
			m_lNodes[i].nBytesReceived = pNode->GetTotalIn();
			m_lNodes[i].nBytesSent = pNode->GetTotalOut();
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
			m_lNodes[i].sCountry = "";//TODO: create function in geoiplist to return country name from country code

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
	qDebug() << "OnRemoveNode";

    for( int i = 0; i < m_lNodes.size(); i++ )
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
	
	qDebug() << "UpdateAll";

    if( Network.m_pSection.tryLock(50) )
    {
		// first check if we need to remove some nodes

		for( int i = 0; i < m_lNodes.size(); i++ )
		{
			bool bFound = false;

			for( int j = 0; j < Network.m_lNodes.size(); j++ )
			{
				if( Network.m_lNodes.at(j) == m_lNodes.at(i).pNode )
				{
					bFound = true;
				}
			}

			if( !bFound )
			{
				// not found in network - remove it
				OnRemoveNode(m_lNodes.at(i).pNode);
			}
		}

		// now add missing nodes or update existing

		for( int i = 0; i < Network.m_lNodes.size(); i++ )
		{
			bool bFound = false;

			for( int j = 0; j < m_lNodes.size(); j++ )
			{
				if( Network.m_lNodes.at(i) == m_lNodes.at(j).pNode )
				{
					UpdateNode(m_lNodes.at(j).pNode, false);
					bFound = true;
					break;
				}
			}

			if( !bFound )
			{
				AddNode(Network.m_lNodes.at(i), false);
			}
		}

		Network.m_pSection.unlock();

		QModelIndex idx1 = index(0, 0, QModelIndex());
		QModelIndex idx2 = index(m_lNodes.size(), 10, QModelIndex());
		emit dataChanged(idx1, idx2);
    }
}

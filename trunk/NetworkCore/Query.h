#ifndef QUERY_H
#define QUERY_H

#include <QList>
#include <QString>
#include "types.h"

class G2Packet;

class CQuery
{
	QList<QString>  m_lURNs;
    QString         m_sMetadata;
    quint64         m_nMinimumSize;
    quint64         m_nMaximumSize;
	QString         m_sDescriptiveName;

    QUuid           m_oGUID;

    G2Packet*       m_pCachedPacket;
public:
	CQuery();
	QString DescriptiveName() { return m_sDescriptiveName; }

    void SetGUID(QUuid& guid);

    void AddURN(const char* pURN, quint32 nLength);
    void SetDescriptiveName(QString sDN);
    void SetSizeRestriction(quint64 nMin, quint64 nMax);
    void SetMetadata(QString sMeta);

    G2Packet* ToG2Packet(IPv4_ENDPOINT* pAddr = 0, quint32 nKey = 0);
};

#endif // QUERY_H

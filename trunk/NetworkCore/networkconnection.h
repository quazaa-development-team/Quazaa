//
// networkconnection.h
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

#ifndef NETWORKCONNECTION_H
#define NETWORKCONNECTION_H

#include "types.h"
#include <QTime>
#include <QObject>

class QByteArray;
class QTcpSocket;
class QThread;

class TCPBandwidthMeter
{
public:
	QTime	m_tTime;
	quint32 m_pSlots[20]; // 4slots / 1sec
	quint32 m_nCurrentSlot;
	quint64 m_nTotal;

public:
	TCPBandwidthMeter();

	void	Add(quint32 nBytes);
	quint32	AvgUsage();
	quint32 Usage();

};

class CNetworkConnection : public QObject
{
	Q_OBJECT
public:
	QTcpSocket* m_pSocket;  // sockecik ;)

	// Adres hosta
	IPv4_ENDPOINT   m_oAddress;

	// Bufory I/O
	QByteArray* m_pInput;
	QByteArray* m_pOutput;
	quint64     m_nInputSize;

	bool    m_bInitiated;
	bool    m_bConnected;
	qint32  m_tConnected;

	bool	m_bDelayedClose;


public:
	CNetworkConnection(QObject* parent = 0);
	virtual ~CNetworkConnection();
	void moveToThread(QThread *thread);

public:
	virtual void ConnectTo(IPv4_ENDPOINT oAddress);
	virtual void ConnectTo(QHostAddress oAddress, quint16 nPort);
	virtual void AttachTo(CNetworkConnection* pOther);
	virtual void AcceptFrom(int nHandle);
	virtual void Close(bool bDelayed = false);

public:
	void Write(QByteArray baData)
	{
		Write(baData.constData(), baData.size());
	}

	void Write(const char* szData, quint32 nLength)
	{
		writeData(szData, nLength);
	}

	qint64 Read(char* pData, qint64 nMaxSize = 0)
	{
		return readData(pData, nMaxSize);
	}

	QByteArray Read(qint64 nMaxSize = 0)
	{
		QByteArray baRet;

		baRet.resize(qMin<qint64>(nMaxSize, m_pInput->size()));
		readData(baRet.data(), baRet.size());
		return baRet;
	}

	QByteArray Peek(qint64 nMaxLength = 0)
	{
		return GetInputBuffer()->left(qMin<qint64>(nMaxLength ? nMaxLength : GetInputBuffer()->size(), GetInputBuffer()->size()));
	}

protected:
	virtual qint64 readFromNetwork(qint64 nBytes);
	virtual qint64 writeToNetwork(qint64 nBytes);

protected:
	virtual qint64 readData(char* data, qint64 maxlen);
	virtual qint64 readLineData(char* data, qint64 maxlen);
	virtual qint64 writeData(const char* data, qint64 len);

	void initializeSocket();

public:

	qint64 bytesAvailable();
	qint64 bytesToWrite() const;
	qint64 networkBytesAvailable() const;
	bool isValid() const;
	void setReadBufferSize(qint64 nSize);

	inline IPv4_ENDPOINT GetAddress()
	{
		return m_oAddress;
	}

	inline virtual bool HasData()
	{
		if( !m_pSocket )
			return false;

		bool bRet = false;
		if( m_pInput && !m_pInput->isEmpty() )
			bRet |= true;
		if( m_pOutput && !m_pOutput->isEmpty() )
			bRet |= true;
		if( networkBytesAvailable() )
			bRet |= true;
		return bRet;
	}

	inline virtual QByteArray* GetInputBuffer()
	{
		Q_ASSERT(m_pInput != 0);

		return m_pInput;
	}
	inline virtual QByteArray* GetOutputBuffer()
	{
		Q_ASSERT(m_pOutput != 0);

		return m_pOutput;
	}
signals:
	void connected();
	void readyRead();
	void disconnected();
	void error(QAbstractSocket::SocketError);
	void bytesWritten(qint64);
	void stateChanged(QAbstractSocket::SocketState);
	void aboutToClose();
	void readyToTransfer();

protected slots:
	void OnAboutToClose();


public:
	TCPBandwidthMeter m_mInput;
	TCPBandwidthMeter m_mOutput;

	friend class CRateController;
};


#endif // NETWORKCONNECTION_H

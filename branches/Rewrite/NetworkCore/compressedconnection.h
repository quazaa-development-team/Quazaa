/*
** compressedconnection.h
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef COMPRESSEDCONNECTION_H
#define COMPRESSEDCONNECTION_H

#include "networkconnection.h"
#include <QElapsedTimer>
#include "zlib/zlib.h"


class CBuffer;

class CCompressedConnection : public CNetworkConnection
{
	Q_OBJECT

public:
	z_stream    m_sInput;           // stream-y zlib-a
	z_stream    m_sOutput;
	bool        m_bCompressedInput; // czy kompresja wlaczona?
	bool        m_bCompressedOutput;
	CBuffer* 	m_pZInput;         // bufforki lokalne
	CBuffer* 	m_pZOutput;
	quint64     m_nTotalInput;      // statystyki
	quint64     m_nTotalOutput;
	quint64     m_nNextDeflateFlush;
	bool        m_bOutputPending;
	QElapsedTimer m_tDeflateFlush;
public:
	CCompressedConnection(QObject* parent = 0);
	virtual ~CCompressedConnection();

	bool EnableInputCompression(bool bEnable = true);
	bool EnableOutputCompression(bool bEnable = true);

	virtual qint64 readFromNetwork(qint64 nBytes);
	virtual qint64 writeToNetwork(qint64 nBytes);

protected:
	bool SetupInputStream();
	bool SetupOutputStream();
	void CleanupInputStream();
	void CleanupOutputStream();

	void Inflate();
	void Deflate();

public:
	inline CBuffer* GetInputBuffer()
	{
		return (m_bCompressedInput ? m_pZInput : m_pInput);
	}
	inline CBuffer* GetOutputBuffer()
	{
		return (m_bCompressedOutput ? m_pZOutput : m_pOutput);
	}
	inline virtual bool HasData()
	{
		if( m_bOutputPending )
			return true;

		if(m_pZInput && !m_pZInput->isEmpty())
		{
			return true;
		}
		if(m_pZOutput && !m_pZOutput->isEmpty())
		{
			return true;
		}

		return CNetworkConnection::HasData();
	}

	inline quint64 GetTotalInDecompressed()
	{
		return m_nTotalInput;
	}
	inline quint64 GetTotalOutCompressed()
	{
		return m_nTotalOutput;
	}

signals:

public slots:

};

#endif // COMPRESSEDCONNECTION_H

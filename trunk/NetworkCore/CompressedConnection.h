//
// CompressedConnection.h
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

#ifndef COMPRESSEDCONNECTION_H
#define COMPRESSEDCONNECTION_H

#include "NetworkConnection.h"
#include <QTime>
#include "zlib/zlib.h"


class QByteArray;

class CCompressedConnection : public CNetworkConnection
{
    Q_OBJECT

public:
    z_stream    m_sInput;           // stream-y zlib-a
    z_stream    m_sOutput;
    bool        m_bCompressedInput; // czy kompresja wlaczona?
    bool        m_bCompressedOutput;
    QByteArray* m_pZInput;          // bufforki lokalne
    QByteArray* m_pZOutput;
    quint64     m_nTotalInput;      // statystyki
    quint64     m_nTotalOutput;
    quint64     m_nNextDeflateFlush;
    QTime       m_tDeflateFlush;
    bool        m_bOutputPending;
public:
    CCompressedConnection(QObject *parent = 0);
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
    inline QByteArray* GetInputBuffer()
    {
        return (m_bCompressedInput ? m_pZInput : m_pInput);
    }
    inline QByteArray* GetOutputBuffer()
    {
        return (m_bCompressedOutput ? m_pZOutput : m_pOutput);
    }
    inline virtual bool HasData()
    {
        bool bRet = false;
        if( m_pZInput && m_pZInput->size() )
            bRet |= true;
        if( m_pZOutput && m_pZOutput->size() )
            bRet |= true;

        //bRet |= m_bOutputPending;

        bRet |= CNetworkConnection::HasData();
        return bRet;

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

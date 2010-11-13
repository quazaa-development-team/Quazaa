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

#ifndef FILEHASHER_H
#define FILEHASHER_H

#include "SharedFile.h"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include "ShareManager/SharedFile.h"

class CFileHasher: public QThread
{
	Q_OBJECT
public:
	static QMutex   m_pSection;
	static QQueue<CSharedFilePtr> m_lQueue;
	static CFileHasher** m_pHashers;
	static quint32  m_nMaxHashers;
	static quint32  m_nRunningHashers;

	static QWaitCondition m_oWaitCond;

	bool m_bActive;
public:
	CFileHasher(QObject* parent = 0);
	~CFileHasher();
	static CFileHasher* HashFile(CSharedFilePtr pFile);
	void run();

signals:
	void FileHashed(CSharedFilePtr);
	void QueueEmpty();
};

#endif // FILEHASHER_H

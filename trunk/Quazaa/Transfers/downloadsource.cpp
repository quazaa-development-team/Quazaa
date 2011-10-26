/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
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


#include "downloadsource.h"
#include "queryhit.h"
#if defined(_MSC_VER) && defined(_DEBUG)
	#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
	#define new DEBUG_NEW
#endif
CDownloadSource::CDownloadSource(CQueryHit* pHit)
{
	m_oAddress = pHit->m_pHitInfo->m_oNodeAddress;
	m_bPush = false; // todo
	m_oPushProxies << pHit->m_pHitInfo->m_lNeighbouringHubs;
	m_nProtocol = tpHTTP;
	m_nNetwork = dpGnutella2;
	m_oGUID = pHit->m_pHitInfo->m_oNodeGUID;
	m_lHashes << pHit->m_lHashes;
	m_tNextAccess = time(0);
	m_nFailures = 0;
}


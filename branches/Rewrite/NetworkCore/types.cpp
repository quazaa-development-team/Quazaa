/*
** types.cpp
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

#include "types.h"
#include <QMetaType>

uint qHash(const QUuid& key)
{
	uint nHash = 0;
	nHash ^= key.data1;
	nHash ^= key.data2;
	nHash ^= (key.data3 << 16);
	/*
	The void* cast below is correct,
	this is to avoid warnings like:
	"dereferencing type-punned pointer will break strict-aliasing rules"
	when compiling with -fstrict-aliasing (enabled by default when using -O2)
	*/
	nHash ^= *(quint32*)(void*)&key.data4[0];
	nHash ^= *(quint32*)(void*)&key.data4[4];

	return nHash;
}

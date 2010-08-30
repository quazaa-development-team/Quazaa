//
// types.cpp
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

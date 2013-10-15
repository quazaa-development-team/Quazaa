/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2013.
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

#include "ircuserlistmodel.h"
#include "ircusermodel_p.h"

IrcUserListModel::IrcUserListModel(QObject *parent) :
IrcUserModel(parent)
{
}

IrcUserListModel::~IrcUserListModel()
{
}

QVariant IrcUserListModel::data(const QModelIndex& index, int role) const
{
	if (!channel() || !hasIndex(index.row(), index.column(), index.parent()))
		return QVariant();

	IrcUser* user = static_cast<IrcUser*>(index.internalPointer());
	Q_ASSERT(user);

	switch (role) {
	case Qt::DisplayRole:
		return user->name();
	case Qt::DecorationRole:
		return modeToIcon(user->mode().left(1));
	case Irc::UserRole:
		return QVariant::fromValue(user);
	case Irc::NameRole:
		return user->name();
	case Irc::PrefixRole:
		return user->prefix().left(1);
	case Irc::ModeRole:
		return user->mode().left(1);
	case Irc::TitleRole:
		return user->title();
	}

	return QVariant();
}

QIcon IrcUserListModel::modeToIcon(const QString &mode) const
{
	if(mode == "Y")
	{
		return QIcon(":/Resource/Chat/Immune.png");
	}
	else if(mode == "q")
	{
		return QIcon(":/Resource/Chat/Owner.png");
	}
	else if(mode == "a")
	{
		return QIcon(":/Resource/Chat/Admin.png");
	}
	else if(mode == "o")
	{
		return QIcon(":/Resource/Chat/Op.png");
	}
	else if(mode == "h")
	{
		return QIcon(":/Resource/Chat/HalfOp.png");
	}
	else if(mode == "v")
	{
		return QIcon(":/Resource/Chat/Voice.png");
	}
	else
	{
		return QIcon(":/Resource/Chat/Normal.png");
	}
}

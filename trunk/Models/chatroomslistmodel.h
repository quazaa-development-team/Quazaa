#ifndef CHATROOMSLISTMODEL_H
#define CHATROOMSLISTMODEL_H

#include <QAbstractListModel>

typedef struct
{
	bool selected;
	QString channelName;
	QString roomName;
} sChatRoom;

class ChatRoomsListModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit ChatRoomsListModel(QObject* parent = 0);

signals:

public slots:

};

#endif // CHATROOMSLISTMODEL_H

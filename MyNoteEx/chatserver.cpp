#include "chatserver.h"
#include <QDebug>
#include <QWidget>
ChatServer::ChatServer(MyNote *parent)
    : QObject(parent)
{
    p=parent;
}

ChatServer::~ChatServer()
{}

void ChatServer::getJS()
{
  p->newSchedule();
}

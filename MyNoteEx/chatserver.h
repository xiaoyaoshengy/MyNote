#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include "mynote.h"

class ChatServer : public QObject
{
    Q_OBJECT

public:
    explicit ChatServer(MyNote *parent = nullptr);
    virtual ~ChatServer();

    Q_INVOKABLE void getJS();
private:
    MyNote *p;

};

#endif // CHATSERVER_H

﻿#ifndef WEBSOCKETCLIENTWRAPPER_H
#define WEBSOCKETCLIENTWRAPPER_H

#include <QObject>

#include "websockettransport.h"

QT_BEGIN_NAMESPACE
class QWebSocketServer;
QT_END_NAMESPACE

class WebSocketClientWrapper : public QObject
{
    Q_OBJECT

public:
    WebSocketClientWrapper(QWebSocketServer *server, QObject *parent = nullptr);

signals:
    void clientConnected(WebSocketTransport *client);

private slots:
    void handleNewConnection();

private:
    QWebSocketServer *m_server;
};

#endif // WEBSOCKETCLIENTWRAPPER_H

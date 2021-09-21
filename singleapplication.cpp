#include "singleapplication.h"
#include "mynote.h"

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QFileInfo>

// 定义超时时长
#define TIME_OUT 500

SingleApplication::SingleApplication(int &argc, char **argv) : QApplication(argc, argv)
{
    m_is_running = false;
    w = NULL;
    m_p_local_server = NULL;
    // 取应用程序名作为LocalServer的名字
    m_server_name = QFileInfo(QApplication::applicationFilePath()).fileName();
    initLocalConnection();
}

SingleApplication::~SingleApplication()
{
    if(m_p_local_server && !m_is_running)
    {
        m_p_local_server->close();
        delete m_p_local_server;
        m_p_local_server = nullptr;
    }
}

bool SingleApplication::isRunning()
{
    return m_is_running;
}

void SingleApplication::newLocalConnection()
{
    QLocalSocket *localSocket = m_p_local_server->nextPendingConnection();
    if(localSocket)
    {
        localSocket->waitForReadyRead(TIME_OUT * 2);
        delete localSocket;

        // 其他处理，如：读取启动参数
        activateWindow();
    }
}

void SingleApplication::initLocalConnection()
{
    m_is_running = false;
    QLocalSocket socket;
    socket.connectToServer(m_server_name);
    if(socket.waitForConnected(TIME_OUT))
    {
        m_is_running = true;

        // 其他处理，如：将启动参数发送到服务端
        return;
    }

    // 连接不上服务器，就创建一个
    newLocalServer();
}

void SingleApplication::newLocalServer()
{
    m_p_local_server = new QLocalServer(this);
    connect(m_p_local_server, &QLocalServer::newConnection, this, &SingleApplication::newLocalConnection);
    if(!m_p_local_server->listen(m_server_name))
    {
        // 此时监听失败，可能是程序崩溃时，残留进程服务器导致的，移除
        if(m_p_local_server->serverError() == QAbstractSocket::AddressInUseError)
        {
            m_p_local_server->removeServer(m_server_name);
            // 重新监听
            m_p_local_server->listen(m_server_name);
        }
    }
}

void SingleApplication::activateWindow()
{
    if(w)
    {
        w->raise();
        w->showNormal();
    }
}

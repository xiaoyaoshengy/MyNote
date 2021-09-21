#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QApplication>

class MyNote;
class QLocalServer;

/**
 * @brief The SingleApplication class
 * 单例程序模块，使程序只能运行一个实例
 */
class SingleApplication : public QApplication
{
    Q_OBJECT
public:
    /**
     * @brief SingleApplication 构造函数
     * @param argc 参数数目
     * @param argv 传入参数
     */
    SingleApplication(int &argc, char **argv);

    /**
     * @brief ~SingleApplication 析构函数
     */
    ~SingleApplication();

    /**
     * @brief isRunning
     * 判断是否已有实例正在运行
     * @return 判断结果
     */
    bool isRunning();

public:
    /**
     * @brief w 程序主模块
     */
    MyNote *w;

private slots:
    /**
     * @brief newLocalConnection
     * 当有新连接时触发
     */
    void newLocalConnection();

private:
    /**
     * @brief initLocalConnection
     * 初始化本地连接
     */
    void initLocalConnection();

    /**
     * @brief newLocalServer
     * 创建服务端
     */
    void newLocalServer();

    /**
     * @brief activateWindow
     * 显示窗口
     */
    void activateWindow();

private:
    /**
     * @brief m_p_local_server 本地服务器
     */
    QLocalServer *m_p_local_server;

    /**
     * @brief m_server_name 本地服务器名称
     */
    QString m_server_name;

    /**
     * @brief m_is_running 是否已有实例正在运行
     */
    bool m_is_running;
};

#endif // SINGLEAPPLICATION_H

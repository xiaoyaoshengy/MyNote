#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QObject>
#include <QThread>
#include <QDebug>

class MyNote;
// controller用于启动线程和处理线程执行结果
class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller(MyNote *parent= nullptr);
    ~Controller();

public slots:
    // 处理线程执行的结果
    void handleResults(const int rslt)
    {
        qDebug()<<"receive the resultReady signal---------------------------------";
        qDebug()<<"     current thread ID:"<<QThread::currentThreadId()<<'\n';
        qDebug()<<"     the last result is:"<<rslt;
    }
signals:
    // 发送信号触发线程
    void operate(const int);
private:
    MyNote *p;

};

#endif // CONTROLLER_H


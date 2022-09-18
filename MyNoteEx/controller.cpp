#include "controller.h"
#include "mynote.h"
#include <worker.h>
Controller::Controller(MyNote *parent) : QObject(parent)
{
    p=parent;
    Worker *worker = new Worker;
    worker->setSavePath(p->savePath());
    worker->moveToThread(&workerThread);//调用moveToThread将该任务交给workThread

    connect(this, SIGNAL(operate(const int)), worker, SLOT(doWork(int)));//operate信号发射后启动线程工作

    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater); //该线程结束时销毁

    connect(worker, SIGNAL(resultReady(int)), this, SLOT(handleResults(int)));//线程结束后发送信号，对结果进行处理

    workerThread.start();  //启动线程//发射信号，开始执行

    qDebug()<<"emit the signal to execute!---------------------------------";
    qDebug()<<"     current thread ID:"<<QThread::currentThreadId()<<'\n';
    emit operate(0);
}
//析构函数中调用quit()函数结束线程
Controller::~Controller()
{
    workerThread.quit();
    workerThread.wait();
}


// work定义了线程要执行的工作
#pragma execution_character_set("utf-8")
#ifndef WORKER_H
#define WORKER_H
#include <QObject>
#include<QDebug>
#include<QThread>
#include <QApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include<QCoreApplication>
#include <QThread>
#include "controller.h"
#include "lobal.h"
#include <QSysInfo>
#include<QThread>
#include <cstdlib>
#include <iostream>
#include<QString>
#include"mainwindow.h"
#include<QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
using std::cout;


class Worker:public QObject
{
    Q_OBJECT
public:
    Worker(QObject* parent = nullptr){
    }
    void onWriteText();
    void onWriteMessage(QString Result);
    void setSavePath(QString);
private:
   QString SavePath;
   QString Address;
   QString Time;
   QString Result;

public slots:
     // doWork定义了线程要执行的操作
void doWork(int parameter);

signals:
    void resultReady(const int result);// 线程完成工作时发送的信号
};

#endif // WORKER_H


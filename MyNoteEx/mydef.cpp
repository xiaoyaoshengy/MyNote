#include "mydef.h"

#include <QString>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>
#include <QDateTime>

MyDef::MyDef()
{

}

void MyDef::printLog(const char* file, int line, QString err)
{
    qDebug().noquote() << QString(file) + QString::fromLocal8Bit("(") + QString::number(line) + QString::fromLocal8Bit("): ") + err;
}

void MyDef::exportErrorMsg(QString err)
{
    QFile errorLog(QCoreApplication::applicationDirPath() + QString::fromLocal8Bit("/error.log"));
    if(!errorLog.open(QFile::Append | QFile::Text))
    {
        //调试
        printLog(__FILE__, __LINE__, QString::fromLocal8Bit("无法打开文件"));
        return;
    }
    QTextStream errorLogStream(&errorLog);
    errorLogStream.setAutoDetectUnicode(true);
    errorLogStream << QDateTime::currentDateTime().toString(QString::fromLocal8Bit("yyyy-MM-dd hh:mm:ss")) << "\t";
    errorLogStream << err << Qt::endl;
}

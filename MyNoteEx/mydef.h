#ifndef MYDEF_H
#define MYDEF_H

#include <QDebug>
#include <QString>
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>
#include <QDateTime>

/**
 * @brief printLog 打印信息
 * 在控制台打印有关文件对应行输出的信息
 * @param file 调用输出命令的文件，具体为宏__FILE__
 * @param line 调用输出命令的语句在文件中的所在行，具体为宏__LINE__
 * @param msg 要输出的信息
 */
static void printLog(const char* file, int line, QString msg)
{
    qDebug().noquote() << file << "(" << line << "): " << msg;
}

/**
 * @brief exportMsg 导出信息
 * 导出信息到程序同级目录的message.log文件中
 * @param msg 待导出的信息
 */
static void exportMsg(QString msg)
{
    QFile logFile(QCoreApplication::applicationDirPath() + QString::fromLocal8Bit("/message.log"));
    if(!logFile.open(QFile::Append | QFile::Text))
    {
        printLog(__FILE__, __LINE__, QString::fromLocal8Bit("无法打开log文件"));
        return;
    }
    QTextStream stream(&logFile);
    stream.setAutoDetectUnicode(true);
    stream << QDateTime::currentDateTime().toString(QString::fromLocal8Bit("yyyy-MM-dd hh:mm:ss: ")) << msg << Qt::endl;
}

#endif // MYDEF_H

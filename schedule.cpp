#include "schedule.h"
#include "mydef.h"
#include "mynote.h"

#include <QDir>
#include <QCoreApplication>
#include <QFile>
#include <QMessageBox>
#include <QSoundEffect>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

Schedule::Schedule(MyNote *parent) : QObject(parent)
{
    //初始化变量
    m_id = QUuid();
    m_title = QString::fromLocal8Bit("");
    m_time = QDateTime::currentDateTime();
    m_address = QString::fromLocal8Bit("");
    m_note = QTextEdit().toHtml();
    m_is_set_clock = false;
    m_p_parent = parent;
    m_old_time = QDateTime::currentDateTime();
    m_timerId = -1;
}

Schedule::~Schedule()
{
}

QString Schedule::title()
{
    return m_title;
}

void Schedule::setTitle(QString title)
{
    m_title = title;
}

QDateTime Schedule::time()
{
    return m_time;
}

void Schedule::setTime(QDateTime time,bool isFirstSaved)
{
    if(!isFirstSaved){
        m_old_time = m_time;
        m_time = time;
    }
    else{
        m_time = time;
        m_old_time = time;
    }
}

QString Schedule::address()
{
    return m_address;
}

void Schedule::setAddress(QString address)
{
    m_address = address;
}

QString Schedule::note()
{
    return m_note;
}

void Schedule::setNote(QString note)
{
    m_note = note;
}

bool Schedule::isSetClock()
{
    return m_is_set_clock;
}

void Schedule::setClock(bool set)
{
    m_is_set_clock = set;
}

QUuid Schedule::id()
{
    return m_id;
}

void Schedule::setId(QUuid id)
{
    m_id = id;
}

void Schedule::saveSchedule(bool isFirstSaved)
{
    QString savePath = m_p_parent->savePath();
    //检查存储文件夹是否存在
    QDir dir(savePath);
    //如果存储文件夹不存在，创建
    if(!dir.exists())
    {
        //如果文件夹创建失败，错误处理
        if(!dir.mkdir(savePath))
        {
            QString err = QString::fromLocal8Bit("创建目录失败");
            //调试
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
            return;
        }
    }

    if(m_old_time.date() == m_time.date())
    {
        //创建或打开日程文件
        QFile schedule(savePath + QString::fromLocal8Bit("/") + m_time.toString(QString::fromLocal8Bit("yyyy-MM-dd")) + QString::fromLocal8Bit(".json"));
        //如果日程文件存在，读取内容
        QJsonDocument jsonDocument;
        QJsonArray tasks;
        if(schedule.exists())
        {
            //如果文件打开失败，错误处理
            if(!schedule.open(QFile::ReadOnly))
            {
                QString err = schedule.fileName() + QString::fromLocal8Bit("文件打开失败");
                //调试
                printLog(__FILE__, __LINE__, err);
                exportMsg(err);
                return;
            }
            //读取内容
            QByteArray json = schedule.readAll();
            schedule.close();
            QJsonParseError *jsonError = nullptr;
            jsonDocument = QJsonDocument::fromJson(json, jsonError);
            //如果内容读取失败，错误处理
            if(jsonDocument.isNull())
            {
                //错误处理
                if(jsonError != nullptr)
                {
                    QString err = schedule.fileName() + QString::fromLocal8Bit(": ") + jsonError->errorString();
                    //调试
                    printLog(__FILE__, __LINE__, err);
                    exportMsg(err);
                }
                else
                {
                    QString err = schedule.fileName() + QString::fromLocal8Bit(": 未知错误");
                    //调试
                    printLog(__FILE__, __LINE__, err);
                    exportMsg(err);
                }
                return;
            }
            tasks = jsonDocument.array();
        }

        //如果是第一次保存，创建Task
        if(isFirstSaved)
        {
            //Task
            QJsonObject task;

            //id
            task.insert(QString::fromLocal8Bit("id"), m_id.toString());

            //Title
            task.insert(QString::fromLocal8Bit("Title"), m_title);

            //Time
            task.insert(QString::fromLocal8Bit("Time"), m_time.toString(QString::fromLocal8Bit("hh-mm")));

            //IsSetClock
            task.insert(QString::fromLocal8Bit("IsSetClock"), m_is_set_clock);

            //Address
            task.insert(QString::fromLocal8Bit("Address"), m_address);

            //Note
            task.insert(QString::fromLocal8Bit("Note"), m_note);

            tasks.append(task);

            m_p_parent->addSchedule(this);
        }
        //如果不是第一次保存，修改Task
        else
        {
            if(tasks.size() == 0)
            {
                //调试
                QString err = schedule.fileName() + QString::fromLocal8Bit(": 文件损坏");
                printLog(__FILE__, __LINE__, err);
                exportMsg(err);
            }
            for(int i = 0;i < tasks.size();i++)
            {
                QJsonValue value = tasks.at(i);
                if(value.isObject())
                {
                    QJsonObject task = value.toObject();
                    QJsonValue id = task.value(QString::fromLocal8Bit("id"));
                    if(id.isString() && id.toString() == m_id.toString())
                    {
                        task.insert(QString::fromLocal8Bit("Title"), m_title);
                        task.insert(QString::fromLocal8Bit("Time"), m_time.toString(QString::fromLocal8Bit("hh-mm")));
                        task.insert(QString::fromLocal8Bit("IsSetClock"), m_is_set_clock);
                        task.insert(QString::fromLocal8Bit("Address"), m_address);
                        task.insert(QString::fromLocal8Bit("Note"), m_note);
                        tasks.replace(i, task);
                        break;
                    }
                }
            }
        }

        //保存到QJsonDocument类中
        jsonDocument.setArray(tasks);

        //写入文件
        if(!schedule.open(QFile::WriteOnly))
        {
            QString err = schedule.fileName() + QString::fromLocal8Bit("文件打开失败");
            //调试
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
            return;
        }
        schedule.write(jsonDocument.toJson());
        schedule.close();
    }
    else
    {
        QFile oldSchedule(savePath + QString("/") + m_old_time.toString(QString("yyyy-MM-dd")) + QString(".json"));
        QJsonDocument jsonDocument;
        QJsonArray tasks;
        if(oldSchedule.exists())
        {
            if(!oldSchedule.open(QFile::ReadOnly))
            {
                QString err = oldSchedule.fileName() + QString::fromLocal8Bit("文件无法打开");
                printLog(__FILE__, __LINE__, err);
                exportMsg(err);
                return;
            }

            QByteArray json = oldSchedule.readAll();
            oldSchedule.close();

            QJsonParseError* jsonError = nullptr;
            jsonDocument = QJsonDocument::fromJson(json, jsonError);
            if(jsonDocument.isNull())
            {
                if(jsonError != nullptr)
                {
                    QString err = oldSchedule.fileName() + QString(": ") + jsonError->errorString();
                    printLog(__FILE__, __LINE__, err);
                    exportMsg(err);
                }
                else
                {
                    QString err = oldSchedule.fileName() + QString::fromLocal8Bit(": 未知错误");
                    printLog(__FILE__, __LINE__, err);
                    exportMsg(err);
                }
                return;
            }

            tasks = jsonDocument.array();

            for(int i = 0; i < tasks.size(); i++)
            {
                QJsonValue value = tasks.at(i);
                if(value.isObject())
                {
                    QJsonObject task = value.toObject();
                    QJsonValue id = task.value(QString("id"));
                    if(id.isString() && id.toString() == m_id.toString())
                    {
                        tasks.removeAt(i);
                        break;
                    }
                }
            }

            jsonDocument.setArray(tasks);
            if(!oldSchedule.open(QFile::WriteOnly))
            {
                QString err = oldSchedule.fileName() + QString::fromLocal8Bit("文件打开失败");
                printLog(__FILE__, __LINE__, err);
                exportMsg(err);
                return;
            }
            oldSchedule.write(jsonDocument.toJson());
            oldSchedule.close();
        }
        QFile schedule(savePath + QString("/") + m_time.toString(QString("yyyy-MM-dd")) + QString(".json"));
        if(schedule.exists())
        {
            if(!schedule.open(QFile::ReadOnly))
            {
                QString err = schedule.fileName() + QString::fromLocal8Bit("文件无法打开");
                printLog(__FILE__, __LINE__, err);
                exportMsg(err);
                return;
            }

            QByteArray json = schedule.readAll();
            schedule.close();

            QJsonParseError *jsonError = nullptr;
            jsonDocument = QJsonDocument::fromJson(json, jsonError);
            if(jsonDocument.isNull())
            {
                if(jsonError != nullptr)
                {
                    QString err = schedule.fileName() + QString(": ") + jsonError->errorString();
                    printLog(__FILE__, __LINE__, err);
                    exportMsg(err);
                }
                else
                {
                    QString err = schedule.fileName() + QString::fromLocal8Bit(": 未知错误");
                    printLog(__FILE__, __LINE__, err);
                    exportMsg(err);
                }
                return;
            }

            tasks = jsonDocument.array();
        }
        //Task
        QJsonObject task;
        //id
        task.insert(QString("id"), m_id.toString());
        //Title
        task.insert(QString("Title"), m_title);
        //Time
        task.insert(QString("Time"), m_time.toString(QString("hh-mm")));
        //IsSetClock
        task.insert(QString("IsSetClock"), m_is_set_clock);
        //Address
        task.insert(QString("Address"), m_address);
        //Note
        task.insert(QString("Note"), m_note);

        tasks.append(task);

        jsonDocument.setArray(tasks);
        if(!schedule.open(QFile::WriteOnly))
        {
            QString err = schedule.fileName() + QString::fromLocal8Bit("文件打开失败");
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
            return;
        }
        schedule.write(jsonDocument.toJson());
        schedule.close();
    }

    if(!m_p_parent->modifySchedule(m_id))
    {
        QString err = QString::fromLocal8Bit("数据修改失败");
        //调试
        printLog(__FILE__, __LINE__, err);
        exportMsg(err);
    }

    m_old_time = m_time;
}

QDateTime Schedule::oldTime()
{
    return m_old_time;
}

void Schedule::deleteSchedule()
{
    QString savePath = m_p_parent->savePath();
    //检查存储文件夹是否存在
    QDir dir(savePath);
    //如果存储文件夹不存在，创建
    if(!dir.exists())
    {
        //如果文件夹创建失败，错误处理
        if(!dir.mkdir(savePath))
        {
            QString err = QString::fromLocal8Bit("创建目录失败");
            //调试
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
            return;
        }
    }
    //创建或打开日程文件
    QFile schedule(savePath + QString::fromLocal8Bit("/") + m_time.toString(QString::fromLocal8Bit("yyyy-MM-dd")) + QString::fromLocal8Bit(".json"));
    //如果日程文件存在，读取内容
    QJsonDocument jsonDocument;
    QJsonArray tasks;
    if(schedule.exists())
    {
        //如果文件打开失败，错误处理
        if(!schedule.open(QFile::ReadOnly))
        {
            QString err = schedule.fileName() + QString::fromLocal8Bit("文件打开失败");
            //调试
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
            return;
        }
        //读取内容
        QByteArray json = schedule.readAll();
        schedule.close();

        QJsonParseError *jsonError = nullptr;
        jsonDocument = QJsonDocument::fromJson(json, jsonError);
        //如果内容读取失败，错误处理
        if(jsonDocument.isNull())
        {
            //错误处理
            if(jsonError != nullptr)
            {
                QString err = schedule.fileName() + QString::fromLocal8Bit(": ") + jsonError->errorString();
                //调试
                printLog(__FILE__, __LINE__, err);
                exportMsg(err);
            }
            else
            {
                QString err = schedule.fileName() + QString::fromLocal8Bit(": 未知错误");
                //调试
                printLog(__FILE__, __LINE__, err);
                exportMsg(err);
            }
            return;
        }
    }
    tasks = jsonDocument.array();
    for(int i = 0;i < tasks.size();i++)
    {
        QJsonValue value = tasks.at(i);
        if(value.isObject())
        {
            QJsonObject task = value.toObject();
            QJsonValue id = task.value(QString::fromLocal8Bit("id"));
            if(id.isString() && id.toString() == m_id.toString())
            {
                tasks.removeAt(i);
                break;
            }
        }
    }

    //更新QJsonDocument类
    jsonDocument.setArray(tasks);

    //写入文件
    if(!schedule.open(QFile::WriteOnly))
    {
        QString err = schedule.fileName() + QString::fromLocal8Bit("文件打开失败");
        //调试
        printLog(__FILE__, __LINE__, err);
        exportMsg(err);
        return;
    }
    schedule.write(jsonDocument.toJson());
    schedule.close();

    if(m_is_set_clock && m_timerId != -1)
        killTimer(m_timerId);
}

void Schedule::notifySchedule()
{
    QSoundEffect soundEffect;
    soundEffect.setSource(QUrl::fromLocalFile(QString::fromLocal8Bit(":/sounds/bell.wav")));
    soundEffect.setLoopCount(QSoundEffect::Infinite);
    soundEffect.setVolume(0.5f);
    soundEffect.play();
    QMessageBox::StandardButton btn = QMessageBox::warning(m_p_parent, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("日程到期：\n日程：\n") + m_title + QString::fromLocal8Bit("\n地点：\n") + m_address);
    m_is_set_clock = false;
    m_timerId = -1;
    if(btn == QMessageBox::Ok)
    {
        soundEffect.stop();
        m_p_parent->showSchedule(this);
    }
}

void Schedule::timerEvent(QTimerEvent *event)
{
    if(m_timerId != -1 && m_timerId == event->timerId())
    {
        killTimer(m_timerId);
        notifySchedule();
    }
}

void Schedule::addClock(QDateTime destinationTime)
{
    qint64 timeLength = QDateTime::currentDateTime().msecsTo(destinationTime);
    if(timeLength <= 0)
    {
        m_is_set_clock = false;
        saveSchedule(false);
        notifySchedule();
    }
    else
        m_timerId = startTimer(timeLength);
}

void Schedule::removeClock()
{
    if(m_is_set_clock && m_timerId != -1)
        killTimer(m_timerId);
    m_is_set_clock = false;
    m_timerId = -1;
}

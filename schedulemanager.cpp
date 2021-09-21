#include "schedulemanager.h"
#include "ui_schedulemanager.h"
#include "mydef.h"
#include "schedule.h"
#include "mynote.h"

#include <QLabel>
#include <QDir>
#include <QFile>
#include <QTreeWidgetItem>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

ScheduleManager::ScheduleManager(MyNote *parent) :
    QMainWindow(parent),
    ui(new Ui::ScheduleManager)
{
    ui->setupUi(this);

    //获取主窗体指针
    m_p_parent = parent;

    //创建状态栏标签
    m_status_bar_label = new QLabel(QString());
    //状态栏添加标签
    ui->statusbar->addWidget(m_status_bar_label, 5);

    //初始化日程树
    iniScheduleTree();

    //链接信号槽
    connect(ui->treeWidget, &QTreeWidget::currentItemChanged, this, &ScheduleManager::onCurrentItemChanged);
    connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, &ScheduleManager::onItemDoubleClicked);
    connect(ui->actionNewSchedule, &QAction::triggered, this, &ScheduleManager::onActionNewScheduleTriggered);
    connect(ui->actionShowSchedule, &QAction::triggered, this, &ScheduleManager::onActionShowScheduleTriggered);
    connect(ui->actionRemoveSchedule, &QAction::triggered, this, &ScheduleManager::onActionDeleteScheduleTriggered);
}

ScheduleManager::~ScheduleManager()
{
    //释放动态内存
    delete m_status_bar_label;
    m_status_bar_label = nullptr;
    QHash<QUuid, Schedule*>::iterator i = m_manager.begin();
    while(!m_manager.isEmpty())
    {
        QUuid id = i.key();
        Schedule* schedule = i.value();
        if(m_manager.remove(id) == 0)
        {
            QString err = QString::fromLocal8Bit("不存在QUuid=") + id.toString() + QString::fromLocal8Bit("的日程");
            //调试
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
        }
        delete schedule;
        i = m_manager.begin();
    }
    delete ui;
}

void ScheduleManager::setSelection(QDate date)
{
    if(!m_manager.isEmpty())
    {
        QList<QTreeWidgetItem*> items = ui->treeWidget->findItems(date.toString(QString::fromLocal8Bit("yyyy-MM-dd")), Qt::MatchExactly, 0);
        if(!items.isEmpty())
            ui->treeWidget->setCurrentItem(items.at(0));
        else
        {
            QTreeWidgetItem* topItem = ui->treeWidget->topLevelItem(0);
            if(topItem == nullptr)
            {
                QString err = QString::fromLocal8Bit("不存在最顶层日程项");
                //调试
                printLog(__FILE__, __LINE__, err);
                exportMsg(err);
                return;
            }
            ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(0));
        }
    }
}

void ScheduleManager::addSchedule(Schedule *schedule)
{
    QString date = schedule->time().toString(QString::fromLocal8Bit("yyyy-MM-dd"));
    QString time = schedule->time().toString(QString::fromLocal8Bit("hh-mm"));
    QString title = schedule->title();
    QUuid id = schedule->id();

    m_manager.insert(id, schedule);
    QList<QTreeWidgetItem*> scheduleItems = ui->treeWidget->findItems(date, Qt::MatchExactly, 0);
    QTreeWidgetItem *scheduleDateItem;
    if(scheduleItems.isEmpty())
    {
        scheduleDateItem = new QTreeWidgetItem(ScheduleManager::Node_Date);
        ui->treeWidget->addTopLevelItem(scheduleDateItem);
        scheduleDateItem->setText(0, date);
    }
    else
    {
        scheduleDateItem = scheduleItems.at(0);
    }

    QTreeWidgetItem *scheduleItem = new QTreeWidgetItem(ScheduleManager::Node_Schedule);
    scheduleItem->setText(0, time);
    scheduleItem->setText(1, title);
    scheduleItem->setData(1, Qt::AccessibleDescriptionRole, QVariant(id));
    scheduleDateItem->addChild(scheduleItem);

    updateTreeWidget();
}

bool ScheduleManager::modifySchedule(QUuid id)
{
    Schedule *schedule = m_manager.value(id);
    QString oldDate = schedule->oldTime().toString(QString::fromLocal8Bit("yyyy-MM-dd"));
    QString title = schedule->title();
    QString newDate = schedule->time().toString(QString::fromLocal8Bit("yyyy-MM-dd"));
    QString newTime = schedule->time().toString(QString::fromLocal8Bit("hh-mm"));

    QList<QTreeWidgetItem*> dateItems = ui->treeWidget->findItems(oldDate, Qt::MatchExactly, 0);
    if(dateItems.isEmpty())
    {
        QString err = QString::fromLocal8Bit("不存在日期为") + oldDate + QString::fromLocal8Bit("的数据项");
        //调试
        printLog(__FILE__, __LINE__, err);
        exportMsg(err);
        return false;
    }
    QTreeWidgetItem* dateItem = dateItems.at(0);
    QList<QTreeWidgetItem*> scheduleItems = dateItem->takeChildren();
    if(scheduleItems.isEmpty())
    {
        QString err = QString::fromLocal8Bit("日期为") + oldDate + QString::fromLocal8Bit("没有日程");
        //调试
        printLog(__FILE__, __LINE__, err);
        exportMsg(err);
        return false;
    }
    for(int i = 0;i < scheduleItems.length();i++)
    {
        QTreeWidgetItem* scheduleItem = scheduleItems.at(i);
        if(scheduleItem->data(1, Qt::AccessibleDescriptionRole) == QVariant(id))
        {
            scheduleItems.removeAt(i);
            dateItem->addChildren(scheduleItems);
            delete scheduleItem;
            scheduleItem = nullptr;

            QList<QTreeWidgetItem*> scheduleItems = ui->treeWidget->findItems(newDate, Qt::MatchExactly, 0);
            QTreeWidgetItem *scheduleDateItem;
            if(scheduleItems.isEmpty())
            {
                scheduleDateItem = new QTreeWidgetItem(ScheduleManager::Node_Date);
                ui->treeWidget->addTopLevelItem(scheduleDateItem);
                scheduleDateItem->setText(0, newDate);
            }
            else
            {
                scheduleDateItem = scheduleItems.at(0);
            }

            QTreeWidgetItem *scheduleItem = new QTreeWidgetItem(ScheduleManager::Node_Schedule);
            scheduleItem->setText(0, newTime);
            scheduleItem->setText(1, title);
            scheduleItem->setData(1, Qt::AccessibleDescriptionRole, QVariant(id));
            scheduleDateItem->addChild(scheduleItem);

            break;
        }
        if(i == scheduleItems.length() - 1)
        {
            QString err = QString::fromLocal8Bit("日期") + oldDate + QString::fromLocal8Bit("下找不到日程：") + title;
            //调试
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
            return false;
        }
    }

    updateTreeWidget();

    return true;
}

void ScheduleManager::onActionNewScheduleTriggered()
{
    QDateTime today = QDateTime::currentDateTime();
    QString todayDate = today.date().toString(QString::fromLocal8Bit("yyyy-MM-dd"));
    QList<QTreeWidgetItem*> items = ui->treeWidget->findItems(todayDate, Qt::MatchExactly, 0);
    if(items.isEmpty())
    {
        QTreeWidgetItem *todayItem = new QTreeWidgetItem(ScheduleManager::Node_Date);
        todayItem->setText(0, todayDate);
        m_p_parent->newSchedule(today.date());
        ui->treeWidget->addTopLevelItem(todayItem);
    }
    else
    {
        QTreeWidgetItem* selectedItem = ui->treeWidget->currentItem();
        if(selectedItem->type() == ScheduleManager::Node_Date)
        {
            QString selectedDate = selectedItem->text(0);
            m_p_parent->newSchedule(QDate::fromString(selectedDate, QString::fromLocal8Bit("yyyy-MM-dd")));
        }
        else
        {
            QString selectedDate = selectedItem->parent()->text(0);
            m_p_parent->newSchedule(QDate::fromString(selectedDate, QString::fromLocal8Bit("yyyy-MM-dd")));
        }
    }
    updateTreeWidget();
}

void ScheduleManager::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);
    if(current == nullptr)
        return;
    if(current->type() == ScheduleManager::Node_Date)
        m_status_bar_label->setText(current->text(0));
    else if(current->type() == ScheduleManager::Node_Schedule)
        m_status_bar_label->setText(current->parent()->text(0) + QString::fromLocal8Bit(" ") + current->text(0) + QString::fromLocal8Bit(" ") + current->text(1));
    else
        m_status_bar_label->setText(QString());
}

void ScheduleManager::onActionShowScheduleTriggered()
{
    QTreeWidgetItem* currentItem = ui->treeWidget->currentItem();
    if(currentItem != nullptr && currentItem->type() == ScheduleManager::Node_Schedule)
    {
        Schedule* schedule = m_manager.value(currentItem->data(1, Qt::AccessibleDescriptionRole).toUuid());
        m_p_parent->showSchedule(schedule);
    }
}

void ScheduleManager::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    if(item == nullptr)
        return;
    if(item->type() == ScheduleManager::Node_Date)
        return;
    Schedule* schedule = m_manager.value(item->data(1, Qt::AccessibleDescriptionRole).toUuid());
    m_p_parent->showSchedule(schedule);
}

void ScheduleManager::onActionDeleteScheduleTriggered()
{
    QTreeWidgetItem* currentItem = ui->treeWidget->currentItem();
    if(currentItem != nullptr && currentItem->type() == ScheduleManager::Node_Schedule)
    {
        Schedule* schedule = m_manager.take(currentItem->data(1, Qt::AccessibleDescriptionRole).toUuid());
        QTreeWidgetItem* parentItem = currentItem->parent();
        parentItem->removeChild(currentItem);
        delete currentItem;
        currentItem = nullptr;
        schedule->deleteSchedule();
        delete schedule;
        schedule = nullptr;
    }
    updateTreeWidget();
}

void ScheduleManager::iniScheduleTree()
{
    QString savePath = m_p_parent->savePath();
    //获取日程文件夹
    QDir dir = QDir(savePath);
    //如果日程文件夹不存在，创建文件夹并设置日程树
    if(!dir.exists())
    {
        //如果日程文件夹创建失败，处理错误
        if(!dir.mkdir(savePath))
        {
            QString err = QString::fromLocal8Bit("文件夹创建失败");
            //调试
            printLog(__FILE__, __LINE__, err);
            exportMsg(err);
        }
    }
    //如果日程文件夹存在且不为空，读取日程内容设置日程树
    else if(!dir.isEmpty())
    {
        //查找文件名为“xxxx-xx-xx.json”的文件
        dir.setNameFilters(QStringList() << QString::fromLocal8Bit("*.json"));
        //如果筛选的结果大于零，遍历每一份文件
        if(dir.count() > 0)
        {
            for(uint i = 0;i < dir.count();i++)
            {
                QString fileName = dir[i];
                QRegExp reg(QString::fromLocal8Bit("[0-9]{4}-[0-9]{2}-[0-9]{2}.json"));
                if(!reg.exactMatch(fileName)){
                    //调试
                    printLog(__FILE__, __LINE__, QString::fromLocal8Bit("文件不匹配：") + fileName);
                    continue;
                }
                //日程日期
                QString date = fileName.left(10);
                //创建日期节点
                QTreeWidgetItem *dateItem = new QTreeWidgetItem(ScheduleManager::Node_Date);
                dateItem->setText(0, date);

                //若日程文件无法打开，错误处理
                QFile file(dir.path() + QString::fromLocal8Bit("/") + fileName);
                if(!file.open(QFile::ReadOnly))
                {
                    QString err = QString::fromLocal8Bit("无法打开文件") + fileName;
                    //调试
                    printLog(__FILE__, __LINE__, err);
                    exportMsg(err);
                    return;
                }
                //读取日程文件
                QByteArray json = file.readAll();
                file.close();

                QJsonParseError *jsonError = nullptr;
                QJsonDocument jsonDocument = QJsonDocument::fromJson(json, jsonError);
                //如果读取失败，错误处理
                if(jsonDocument.isNull())
                {
                    //错误处理
                    if(jsonError != nullptr)
                    {
                        QString err = file.fileName() + QString::fromLocal8Bit(": ") + jsonError->errorString();
                        //调试
                        printLog(__FILE__, __LINE__, err);
                        exportMsg(err);
                    }
                    else
                    {
                        QString err = file.fileName() + QString::fromLocal8Bit(": 未知错误");
                        //调试
                        printLog(__FILE__, __LINE__, err);
                        exportMsg(err);
                    }
                    return;
                }
                //获取所有task
                QJsonArray tasks = jsonDocument.array();
                //遍历所有task
                for(int j = 0;j < tasks.count();j++)
                {
                    //创建日程节点
                    QTreeWidgetItem *task = new QTreeWidgetItem(ScheduleManager::Node_Schedule);

                    //获取task
                    QJsonValue value = tasks.at(j);
                    if(value.isObject())
                    {
                        QJsonObject taskObject = value.toObject();

                        //获取时间
                        QString time = taskObject.value(QString::fromLocal8Bit("Time")).toString();

                        //获取标题
                        QString title = taskObject.value(QString::fromLocal8Bit("Title")).toString();

                        //获取是否设置闹钟
                        bool isSetClock = taskObject.value(QString::fromLocal8Bit("IsSetClock")).toBool();

                        //获取日程唯一标识码
                        QString id = taskObject.value(QString::fromLocal8Bit("id")).toString();

                        //获取地址
                        QString address = taskObject.value(QString::fromLocal8Bit("Address")).toString();

                        //获取备注
                        QString note = taskObject.value(QString::fromLocal8Bit("Note")).toString();

                        //创建Schedule结构并放入管理器中
                        Schedule *schedule = new Schedule(m_p_parent);
                        schedule->setId(QUuid::fromString(id));
                        schedule->setTitle(title);
                        schedule->setTime(QDateTime::fromString(date + QString::fromLocal8Bit("-") + time, QString::fromLocal8Bit("yyyy-MM-dd-hh-mm")));
                        schedule->setAddress(address);
                        schedule->setNote(note);
                        schedule->setClock(isSetClock);
                        if(isSetClock)
                            schedule->addClock(schedule->time());
                        m_manager.insert(schedule->id(), schedule);

                        //设置日程节点
                        task->setText(0, time);
                        task->setText(1, title);
                        task->setData(1, Qt::AccessibleDescriptionRole, QVariant(schedule->id()));
                        //添加日程节点
                        dateItem->addChild(task);
                    }
                    else
                        delete task;
                }
                ui->treeWidget->addTopLevelItem(dateItem);
            }
        }
    }

    updateTreeWidget();
}

void ScheduleManager::updateTreeWidget()
{
    for(int i = 0;i < ui->treeWidget->topLevelItemCount();i++)
    {
        QTreeWidgetItem* topItem = ui->treeWidget->topLevelItem(i);
        if(topItem->childCount() == 0)
        {
            ui->treeWidget->takeTopLevelItem(i);
            i = 0;
        }
    }
}

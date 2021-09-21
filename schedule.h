#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <QObject>
#include <QDateTime>
#include <QTextEdit>
#include <QUuid>

class MyNote;

class Schedule : public QObject
{
    Q_OBJECT

public:
    Schedule(MyNote *parent = nullptr);
    ~Schedule();
    //获取标题
    QString title();
    //设置标题
    void setTitle(QString title = QString::fromLocal8Bit(""));
    //获取时间
    QDateTime time();
    //设置时间
    void setTime(QDateTime time = QDateTime::currentDateTime(),bool isFirstSaved=false);
    //获取地点
    QString address();
    //设置地点
    void setAddress(QString address = QString::fromLocal8Bit(""));
    //获取备注
    QString note();
    //设置备注
    void setNote(QString note = QTextEdit().toHtml());
    //获取闹钟设置
    bool isSetClock();
    //设置闹钟
    void setClock(bool set = false);
    //获取唯一标识码
    QUuid id();
    //设置唯一标识码
    void setId(QUuid id = QUuid());
    //保存日程
    void saveSchedule(bool isFirstSaved);
    //获取旧日程时间
    QDateTime oldTime();
    //删除日程
    void deleteSchedule();
    //日程到期提醒
    void notifySchedule();
    //重新时间到期事件
    void timerEvent(QTimerEvent *event) override;
    //添加时钟
    void addClock(QDateTime destinationTime);
    //移除时钟
    void removeClock();

private:
    //日程标题
    QString m_title;
    //日程时间
    QDateTime m_time;
    //日程地点
    QString m_address;
    //日程备注
    QString m_note;
    //日程是否设置闹钟
    bool m_is_set_clock;
    //唯一标识码
    QUuid m_id;
    //主窗体指针
    MyNote *m_p_parent;
    //旧日程时间
    QDateTime m_old_time;
    //计时器id
    int m_timerId;
};

#endif // SCHEDULE_H

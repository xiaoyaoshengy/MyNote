#ifndef SCHEDULEMANAGER_H
#define SCHEDULEMANAGER_H

#include <QMainWindow>
#include <QHash>

namespace Ui {
class ScheduleManager;
}

class QLabel;
class Schedule;
class MyNote;
class QTreeWidgetItem;

class ScheduleManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScheduleManager(MyNote *parent = nullptr);
    ~ScheduleManager();
    //设置选中项
    void setSelection(QDate date);
    //添加日程
    void addSchedule(Schedule* schedule);
    //修改日程
    bool modifySchedule(QUuid id);

private slots:
    //新建日程事件触发
    void onActionNewScheduleTriggered();
    //当前选择项改变时
    void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    //查看日程事件触发
    void onActionShowScheduleTriggered();
    //双击日程项事件触发
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    //删除日程事件触发
    void onActionDeleteScheduleTriggered();

private:
    Ui::ScheduleManager *ui;
    //状态栏标签
    QLabel *m_status_bar_label;
    //节点类型枚举类
    enum NodeType
    {
        Node_Date = 1001,//必须从1001开始
        Node_Schedule
    };
    //日程哈希表
    QHash<QUuid, Schedule*> m_manager;
    //主窗体指针
    MyNote* m_p_parent;

    //初始化日程树
    void iniScheduleTree();
    //更新日程树
    void updateTreeWidget();
};

#endif // SCHEDULEMANAGER_H

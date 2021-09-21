#ifndef NEWSCHEDULEDIALOG_H
#define NEWSCHEDULEDIALOG_H

#include <QMainWindow>

namespace Ui {
class NewScheduleDialog;
}

class MyNote;
class QLabel;
class Schedule;
class QDateTimeEdit;
class CustomCalendarWidget;

/**
 * @brief The NewScheduleDialog class
 * 新建日程模块
 */
class NewScheduleDialog : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief NewScheduleDialog 构造函数
     * 根据给定日期新建日程
     * @param date 给定新建日程的日期
     * @param parent 父窗体指针
     */
    NewScheduleDialog(QDate date, MyNote *parent = nullptr);

    /**
     * @brief NewScheduleDialog 构造函数
     * 根据给定日程类新建日程
     * @param schedule 给定新建日程的日程类指针
     * @param parent 父窗体指针
     */
    NewScheduleDialog(Schedule* schedule, MyNote *parent = nullptr);

    /**
     * @brief ~NewScheduleDialog 析构函数
     */
    ~NewScheduleDialog();

    /**
     * @brief closeEvent 关闭事件处理函数
     * 重写父类同名函数，实现自定义处理关闭事件
     * @param event 关闭事件
     */
    void closeEvent(QCloseEvent* event) override;

    /**
     * @brief showEvent 显示事件处理函数
     * 重写父类同名函数，实现自定义处理显示事件
     * @param event 显示事件
     */
    void showEvent(QShowEvent *event) override;

private slots:
    /**
     * @brief onActionSaveTriggered
     * 当“保存”触发时的处理函数
     */
    void onActionSaveTriggered();

private:
    /**
     * @brief ui 设计文件类指针
     */
    Ui::NewScheduleDialog *ui;

    /**
     * @brief m_p_parent 主模块
     */
    MyNote *m_p_parent;

    /**
     * @brief m_has_saved 判断日程是否已经保存
     */
    bool m_has_saved;

    /**
     * @brief m_is_first_save 判断日程是否是第一次保存
     */
    bool m_is_first_save;

    /**
     * @brief m_p_schedule 存储信息的日程类指针
     */
    Schedule* m_p_schedule;

    /**
     * @brief m_is_play_close_animation 是否播放关闭窗体动画
     */
    bool m_is_play_close_animation;

    /**
     * @brief m_custom_calendar_widget 自定义的日历组件
     */
    CustomCalendarWidget *m_custom_calendar_widget;

private:
    /**
     * @brief saveSchedule 保存日程
     */
    void saveSchedule();

    /**
     * @brief iniSignalsAndSlots 连接信号槽
     */
    void iniSignalsAndSlots();
};

#endif // NEWSCHEDULEDIALOG_H

#ifndef MYNOTE_H
#define MYNOTE_H

#include "scrolledlabel.h"
#include "calendarlabel.h"

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QDate>

QT_BEGIN_NAMESPACE
namespace Ui { class MyNote; }
QT_END_NAMESPACE

class QMenu;
class NewScheduleDialog;
class QLabel;
class ScheduleManager;
class Schedule;
class SettingDialog;

/**
 * @brief The MyNote class
 * 日程模块，主模块
 */
class MyNote : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief MyNote 构造函数
     * @param parent 父窗体指针
     */
    MyNote(QWidget *parent = nullptr);

    /**
     * @brief ~MyNote 析构函数
     */
    ~MyNote();

    /**
     * @brief closeEvent 关闭事件处理函数
     * 重写父类同名函数，自定义处理关闭事件
     * @param event 接收到的关闭事件
     */
    void closeEvent(QCloseEvent *event) override;

    /**
     * @brief newSchedule 新建日程
     * 处理新建日程事件
     * @param date 新建日程的日期
     */
    void newSchedule(QDate date = QDate::currentDate());

    /**
     * @brief addSchedule
     * 添加日程到日程管理器中
     * @param schedule 待添加的日程类指针
     */
    void addSchedule(Schedule* schedule);

    /**
     * @brief modifySchedule 修改日程
     * 根据日程id修改日程管理器中的日程内容
     * @param id 待修改的日程id
     * @return 是否修改成功
     */
    bool modifySchedule(QUuid id);

    /**
     * @brief showSchedule 显示日程
     * 打开窗体显示日程的具体信息
     * @param schedule 待显示的日程类指针
     */
    void showSchedule(Schedule* schedule);

    /**
     * @brief savePath 获取保存路径
     * @return 保存路径
     */
    QString savePath();

    /**
     * @brief setSavePath 设置保存路径
     * @param savePath 保存路径内容
     */
    void setSavePath(QString savePath);

public slots:
    /**
     * @brief onActionNewScheduleTriggered
     * 当“新建日程”触发时的处理函数
     */
    void onActionNewScheduleTriggered();

    /**
     * @brief onActionShowScheduleTriggered
     * 当“显示日程”触发时的处理函数
     */
    void onActionShowScheduleTriggered();

    /**
     * @brief onActionSettingTriggered
     * 当“打开设置”触发时的处理函数
     */
    void onActionSettingTriggered();

private slots:
    /**
     * @brief showByClickTray
     * 当双击系统托盘图标时的处理函数，实现显示窗体
     * @param reason 系统托盘图标的激活理由
     */
    void showByClickTray(QSystemTrayIcon::ActivationReason reason);

    /**
     * @brief quitApplication
     * 当“退出程序”触发时的处理函数，实现退出程序
     */
    void quitApplication();

    /**
     * @brief onLabelYear1Changed
     * 与自定义的显示年份数字的标签相关的处理函数，当标签内容改变时触发
     * @param newNum 标签改变后的内容
     */
    void onLabelYear1Changed(int newNum);

    /**
     * @brief onLabelYear2Changed
     * 与自定义的显示年份数字的标签相关的处理函数，当标签内容改变时触发
     * @param newNum 标签改变后的内容
     */
    void onLabelYear2Changed(int newNum);

    /**
     * @brief onLabelYear3Changed
     * 与自定义的显示年份数字的标签相关的处理函数，当标签内容改变时触发
     * @param newNum 标签改变后的内容
     */
    void onLabelYear3Changed(int newNum);

    /**
     * @brief onLabelYear4Changed
     * 与自定义的显示年份数字的标签相关的处理函数，当标签内容改变时触发
     * @param newNum 标签改变后的内容
     */
    void onLabelYear4Changed(int newNum);

    /**
     * @brief onLabelMonthChanged
     * 与自定义的显示月份的标签相关的处理函数，当标签内容改变时触发
     * @param newNum 标签改变后的内容
     */
    void onLabelMonthChanged(int newNum);

    /**
     * @brief onCalendarDoubleClicked
     * 当自定义的日历标签双击时的处理函数
     * @param date 鼠标双击位置处的日期
     */
    void onCalendarDoubleClicked(QDate date);

    /**
     * @brief onCalendarSelectedDateChanged
     * 当自定义的日历标签上选中的日期改变时的处理函数
     * @param date 新选中的日期
     */
    void onCalendarSelectedDateChanged(QDate date);

private:
    /**
     * @brief ui 设计文件类指针
     */
    Ui::MyNote *ui;

    /**
     * @brief m_p_sys_tray_icon 系统托盘图标
     */
    QSystemTrayIcon *m_p_sys_tray_icon;

    /**
     * @brief m_p_sys_tray_menu 系统托盘右键菜单
     */
    QMenu *m_p_sys_tray_menu;

    /**
     * @brief m_p_status_bar_label 状态栏标签
     */
    QLabel *m_p_status_bar_label;

    /**
     * @brief m_p_schedule_manager 日程管理器
     */
    ScheduleManager *m_p_schedule_manager;

    /**
     * @brief m_save_path 保存路径
     */
    QString m_save_path;

    /**
     * @brief m_p_setting_dialog 设置模块
     */
    SettingDialog *m_p_setting_dialog;

    /**
     * @brief m_p_month_pixmaps 月份图片组
     */
    QPixmap *m_p_month_pixmaps;

    /**
     * @brief m_p_number_pixmaps 数字图片组
     */
    QPixmap *m_p_number_pixmaps;

    /**
     * @brief m_p_label_month 月份标签
     */
    ScrolledLabel *m_p_label_month;

    /**
     * @brief m_p_label_year_1 年份数字从左起第1位
     */
    ScrolledLabel *m_p_label_year_1;

    /**
     * @brief m_p_label_year_2 年份数字从左起第2位
     */
    ScrolledLabel *m_p_label_year_2;

    /**
     * @brief m_p_label_year_3 年份数字从左起第3位
     */
    ScrolledLabel *m_p_label_year_3;

    /**
     * @brief m_p_label_year_4 年份数字从左起第4位
     */
    ScrolledLabel *m_p_label_year_4;

    /**
     * @brief m_p_label_calendar 自定义的日历标签
     */
    CalendarLabel *m_p_label_calendar;

    /**
     * @brief m_date 自定义日历显示的日期
     */
    QDate m_date;

private:
    /**
     * @brief setYearLabels 设置年份标签
     * @param year 年份数字
     */
    void setYearLabels(int year);

    /**
     * @brief setMonthLabels 设置月份标签
     * @param month 月份数字
     */
    void setMonthLabels(int month);

    /**
     * @brief setDayLabels 设置天数标签
     * @param day 天数数字
     */
    void setDayLabels(int day);

    /**
     * @brief setCalendarLabel 设置日历标签
     * @param date 日历显示日期
     */
    void setCalendarLabel(QDate date);
};
#endif // MYNOTE_H

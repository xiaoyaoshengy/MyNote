#include "mynote.h"
#include "ui_mynote.h"
#include "newscheduledialog.h"
#include "mydef.h"
#include "schedulemanager.h"
#include "schedule.h"
#include "settingdialog.h"

#include <QMenu>
#include <QCloseEvent>
#include <QLabel>
#include <QPointer>

MyNote::MyNote(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyNote)
{
    ui->setupUi(this);

    // 设置系统托盘右键菜单
    m_p_sys_tray_menu = new QMenu(this);
    m_p_sys_tray_menu->addAction(ui->actionNewSchedule);
    m_p_sys_tray_menu->addAction(ui->actionSetting);
    m_p_sys_tray_menu->addSeparator();
    m_p_sys_tray_menu->addAction(ui->actionQuit);

    // 设置系统托盘图标
    m_p_sys_tray_icon = new QSystemTrayIcon(this);
    m_p_sys_tray_icon->setIcon(QIcon(":/img/images/note.png"));
    m_p_sys_tray_icon->setToolTip(QString::fromLocal8Bit("MyNote v1.0.0"));
    m_p_sys_tray_icon->setContextMenu(m_p_sys_tray_menu);
    connect(m_p_sys_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(showByClickTray(QSystemTrayIcon::ActivationReason)));
    m_p_sys_tray_icon->show();
    m_p_sys_tray_icon->showMessage(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("MyNote应用程序已最小化于系统托盘"));

    // 创建设置面板
    m_p_setting_dialog = new SettingDialog(this);

    // 创建日程管理器
    m_p_schedule_manager = new ScheduleManager(this);

    // 设置自定义日历
    m_date = QDate::currentDate();
    // 设置年份标签
    m_p_number_pixmaps = new QPixmap[10];
    for(int i = 0; i < 10; i++)
        m_p_number_pixmaps[i] = QPixmap(QString(":/img/images/Number/") + QString::number(i) + QString("_red.png"));
    m_p_label_year_1 = new ScrolledLabel(ui->widget_calendar);
    m_p_label_year_1->setGeometry(QRect(430, 30, 20, 40));
    m_p_label_year_1->setPixmaps(m_p_number_pixmaps, 10);
    m_p_label_year_2 = new ScrolledLabel(ui->widget_calendar);
    m_p_label_year_2->setGeometry(QRect(455, 30, 20, 40));
    m_p_label_year_2->setPixmaps(m_p_number_pixmaps, 10);
    m_p_label_year_3 = new ScrolledLabel(ui->widget_calendar);
    m_p_label_year_3->setGeometry(QRect(480, 30, 20, 40));
    m_p_label_year_3->setPixmaps(m_p_number_pixmaps, 10);
    m_p_label_year_4 = new ScrolledLabel(ui->widget_calendar);
    m_p_label_year_4->setGeometry(QRect(505, 30, 20, 40));
    m_p_label_year_4->setPixmaps(m_p_number_pixmaps, 10);
    setYearLabels(m_date.year());
    m_p_label_year_1->show();
    m_p_label_year_2->show();
    m_p_label_year_3->show();
    m_p_label_year_4->show();
    connect(m_p_label_year_1, &ScrolledLabel::pixmapNumChanged, this, &MyNote::onLabelYear1Changed);
    connect(m_p_label_year_2, &ScrolledLabel::pixmapNumChanged, this, &MyNote::onLabelYear2Changed);
    connect(m_p_label_year_3, &ScrolledLabel::pixmapNumChanged, this, &MyNote::onLabelYear3Changed);
    connect(m_p_label_year_4, &ScrolledLabel::pixmapNumChanged, this, &MyNote::onLabelYear4Changed);
    // 设置日期标签
    setDayLabels(m_date.day());
    // 设置月份标签
    m_p_label_month = new ScrolledLabel(ui->widget_calendar);
    m_p_label_month->setGeometry(QRect(30, 30, 200, 40));
    m_p_month_pixmaps = new QPixmap[12];
    for(int i = 0; i < 12; i++)
        m_p_month_pixmaps[i] = QPixmap(QString(":/img/images/Month/") + QString::number(i + 1) + QString(".png"));
    m_p_label_month->setPixmaps(m_p_month_pixmaps, 12);
    setMonthLabels(m_date.month());
    m_p_label_month->show();
    connect(m_p_label_month, &ScrolledLabel::pixmapNumChanged, this, &MyNote::onLabelMonthChanged);
    // 设置日历标签
    m_p_label_calendar = new CalendarLabel(ui->widget_calendar);
    m_p_label_calendar->setGeometry(QRect(185, 115, m_p_label_calendar->width(), m_p_label_calendar->height()));
    m_p_label_calendar->show();
    connect(m_p_label_calendar, &CalendarLabel::doubleClicked, this, &MyNote::onCalendarDoubleClicked);
    connect(m_p_label_calendar, &CalendarLabel::selectedDateChanged, this, &MyNote::onCalendarSelectedDateChanged);

    // 设置状态栏标签
    m_p_status_bar_label = new QLabel;
    m_p_status_bar_label->setText(m_p_label_calendar->selectedDate().toString(QString::fromLocal8Bit("yyyy年MM月dd日 ddd")));
    ui->statusbar->addWidget(m_p_status_bar_label, 5);

    // 链接信号槽
    connect(ui->actionQuit, &QAction::triggered, this, &MyNote::quitApplication);
    connect(ui->actionNewSchedule, &QAction::triggered, this, &MyNote::onActionNewScheduleTriggered);
    connect(ui->actionSetting, &QAction::triggered, this, &MyNote::onActionSettingTriggered);
    connect(ui->actionManageSchedule, &QAction::triggered, this, &MyNote::onActionShowScheduleTriggered);
}

MyNote::~MyNote()
{
    // 释放“状态栏标签”指针
    if(m_p_status_bar_label != nullptr)
        delete m_p_status_bar_label;

    // 释放“系统托盘右键菜单”和“系统托盘图标”指针
    if(m_p_sys_tray_menu != nullptr)
        delete m_p_sys_tray_menu;
    if(m_p_sys_tray_icon != nullptr)
        delete m_p_sys_tray_icon;

    // 释放自定义日历相关指针
    if(m_p_month_pixmaps != nullptr)
        delete []m_p_month_pixmaps;
    if(m_p_number_pixmaps != nullptr)
        delete []m_p_number_pixmaps;
    if(m_p_label_month != nullptr)
        delete m_p_label_month;
    if(m_p_label_year_1 != nullptr)
        delete m_p_label_year_1;
    if(m_p_label_year_2 != nullptr)
        delete m_p_label_year_2;
    if(m_p_label_year_3 != nullptr)
        delete m_p_label_year_3;
    if(m_p_label_year_4 != nullptr)
        delete m_p_label_year_4;
    if(m_p_label_calendar != nullptr)
        delete m_p_label_calendar;

    delete ui;
}

void MyNote::closeEvent(QCloseEvent *event)
{
    // 忽略“关闭”事件，改为隐藏窗体
    event->ignore();
    hide();
}

void MyNote::newSchedule(QDate date)
{
    QPointer<NewScheduleDialog> dialog = new NewScheduleDialog(date, this);
    dialog->show();
}

void MyNote::addSchedule(Schedule *schedule)
{
    m_p_schedule_manager->addSchedule(schedule);
}

bool MyNote::modifySchedule(QUuid id)
{
    return m_p_schedule_manager->modifySchedule(id);
}

void MyNote::showSchedule(Schedule *schedule)
{
    QPointer<NewScheduleDialog> dialog = new NewScheduleDialog(schedule, this);
    dialog->show();
}

QString MyNote::savePath()
{
    return m_save_path;
}

void MyNote::setSavePath(QString savePath)
{
    m_save_path = savePath;
}

void MyNote::showByClickTray(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick)
        show();
}

void MyNote::onActionNewScheduleTriggered()
{
    newSchedule(m_p_label_calendar->selectedDate());
}

void MyNote::quitApplication()
{
    // 释放日程管理器指针
    if(m_p_schedule_manager != nullptr)
        delete m_p_schedule_manager;
    m_p_schedule_manager = nullptr;
    // 释放“设置”窗体指针
    if(m_p_setting_dialog != nullptr)
        delete m_p_setting_dialog;
    m_p_setting_dialog = nullptr;
    // 退出程序
    QCoreApplication::quit();
}

void MyNote::onCalendarSelectedDateChanged(QDate date)
{
    m_p_status_bar_label->setText(date.toString(QString::fromLocal8Bit("yyyy年MM月dd日 ddd")));
    setDayLabels(date.day());
    m_date = date;
}

void MyNote::setYearLabels(int year)
{
    // 检查合法性
    if(year > 9999 || year < 0)
        return;

    int year1 = year / 1000;
    int year2 = year % 1000 / 100;
    int year3 = year % 1000 % 100 / 10;
    int year4 = year % 1000 % 100 % 10;

    m_p_label_year_1->setPixmapNum(year1);
    m_p_label_year_2->setPixmapNum(year2);
    m_p_label_year_3->setPixmapNum(year3);
    m_p_label_year_4->setPixmapNum(year4);
}

void MyNote::setMonthLabels(int month)
{
    // 检查合法性
    if(month > 12 || month < 1)
        return;

    m_p_label_month->setPixmapNum(month - 1);
}

void MyNote::setDayLabels(int day)
{
    // 检查合法性
    if(day > m_date.daysInMonth() || day < 1)
        return;

    int day1 = day / 10;
    int day2 = day % 10;

    ui->label_day_1->setPixmap(QPixmap(QString(":/img/images/Number/") + QString::number(day1) + QString("_yellow.png")));
    ui->label_day_2->setPixmap(QPixmap(QString(":/img/images/Number/") + QString::number(day2) + QString("_yellow.png")));
}

void MyNote::setCalendarLabel(QDate date)
{
    // 检查合法性
    if(!date.isValid())
        return;

    m_p_label_calendar->setDate(date);
}

void MyNote::onCalendarDoubleClicked(QDate date)
{
    m_p_schedule_manager->show();
    m_p_schedule_manager->setSelection(date);
}

void MyNote::onActionSettingTriggered()
{
    m_p_setting_dialog->show();
    m_p_setting_dialog->setSavePath(m_save_path);
}

void MyNote::onActionShowScheduleTriggered()
{
    QDate curDate = m_p_label_calendar->selectedDate();
    m_p_schedule_manager->show();
    m_p_schedule_manager->setSelection(curDate);
}

void MyNote::onLabelYear1Changed(int newNum)
{
    int year = newNum * 1000 + m_date.year() % 1000;
    m_date.setDate(year, m_date.month(), m_date.day());
    if(!m_date.isValid())
    {
        m_date = QDate::currentDate();
        setYearLabels(m_date.year());
        return;
    }
    m_p_label_calendar->setDate(m_date);
}

void MyNote::onLabelYear2Changed(int newNum)
{
    int year = m_date.year();
    year = year / 1000 * 1000 + newNum * 100 + year % 1000 % 100;
    m_date.setDate(year, m_date.month(), m_date.day());
    if(!m_date.isValid())
    {
        m_date = QDate::currentDate();
        setYearLabels(m_date.year());
        return;
    }
    m_p_label_calendar->setDate(m_date);
}

void MyNote::onLabelYear3Changed(int newNum)
{
    int year = m_date.year();
    year = year / 100 * 100 + newNum * 10 + year % 1000 % 100 % 10;
    m_date.setDate(year, m_date.month(), m_date.day());
    if(!m_date.isValid())
    {
        m_date = QDate::currentDate();
        setYearLabels(m_date.year());
        return;
    }
    m_p_label_calendar->setDate(m_date);
}

void MyNote::onLabelYear4Changed(int newNum)
{
    int year = m_date.year() / 10 * 10 + newNum;
    m_date.setDate(year, m_date.month(), m_date.day());
    if(!m_date.isValid())
    {
        m_date = QDate::currentDate();
        setYearLabels(m_date.year());
        return;
    }
    m_p_label_calendar->setDate(m_date);
}

void MyNote::onLabelMonthChanged(int newNum)
{
    m_date.setDate(m_date.year(), newNum + 1, m_date.day());
    m_p_label_calendar->setDate(m_date);
}

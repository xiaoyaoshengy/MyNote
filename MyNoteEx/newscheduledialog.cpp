#include "newscheduledialog.h"
#include "ui_newscheduledialog.h"
#include "mynote.h"
#include "mydef.h"
#include "schedule.h"
#include "customcalendarwidget.h"

#include <QMessageBox>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QLabel>
#include <QCursor>
#include <QPropertyAnimation>
#include <QShowEvent>

NewScheduleDialog::NewScheduleDialog(QDate date, MyNote *parent) :
    QMainWindow(parent),
    ui(new Ui::NewScheduleDialog)
{
    ui->setupUi(this);

    m_p_parent = parent;
    m_has_saved = true;
    m_is_first_save = true;
    m_is_play_close_animation = true;

    //创建日程指针
    m_p_schedule = new Schedule(parent);
    m_p_schedule->setId(QUuid::createUuid());

    //初始化ui->dateTimeEditTime
    ui->dateTimeEditTime->setDate(date);
    ui->dateTimeEditTime->setTime(QTime::currentTime());
    m_custom_calendar_widget = new CustomCalendarWidget(ui->dateTimeEditTime);
    ui->dateTimeEditTime->setCalendarWidget(m_custom_calendar_widget);
    m_custom_calendar_widget->disconnect(SIGNAL(selectionChanged()));
    m_custom_calendar_widget->disconnect(SIGNAL(clicked(QDate)));

    //链接信号槽
    iniSignalsAndSlots();
}

NewScheduleDialog::NewScheduleDialog(Schedule* schedule, MyNote *parent) : QMainWindow(parent), ui(new Ui::NewScheduleDialog)
{
    ui->setupUi(this);
    //获取主窗体指针
    m_p_parent = parent;

    //初始化m_has_saved
    m_has_saved = true;

    //初始化m_is_first_save
    m_is_first_save = false;

    //初始化m_is_play_close_animation
    m_is_play_close_animation = true;

    //根据日程schedule设置“新建日程”窗体中的各个组件内容
    ui->lineEditTitle->setText(schedule->title());
    ui->dateTimeEditTime->setDateTime(schedule->time());
    ui->checkBoxClock->setChecked(schedule->isSetClock());
    ui->plainTextEditAddress->setPlainText(schedule->address());
    ui->textEditNote->setHtml(schedule->note());

    //初始化ui->dateTimeEditTime
    m_custom_calendar_widget = new CustomCalendarWidget(ui->dateTimeEditTime);
    ui->dateTimeEditTime->setCalendarWidget(m_custom_calendar_widget);
    m_custom_calendar_widget->disconnect(SIGNAL(selectionChanged()));
    m_custom_calendar_widget->disconnect(SIGNAL(clicked(QDate)));

    //关联此窗体的日程指针
    m_p_schedule = schedule;

    //链接信号槽
    iniSignalsAndSlots();
}

NewScheduleDialog::~NewScheduleDialog()
{
    delete m_custom_calendar_widget;
    delete ui;
}

void NewScheduleDialog::closeEvent(QCloseEvent *event)
{
    //当“新建日程”窗体内容未保存时，弹出对话框询问是否保存
    if(!m_has_saved && m_is_play_close_animation)
    {
        QMessageBox::StandardButton btn = QMessageBox::question(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("日程尚未保存，是否保存？"), QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel);
        if(btn == QMessageBox::Ok)
            //保存日程
            saveSchedule();
        else if(btn == QMessageBox::Cancel)
            //若取消，返回关闭事件触发以前的状态
            return;
    }

    if(m_is_play_close_animation)
    {
        setMinimumHeight(0);
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        animation->setStartValue(geometry());
        animation->setEndValue(QRect(x(), y() + height() / 2, width(), 0));
        animation->setDuration(300);
        m_is_play_close_animation = false;
        connect(animation, &QPropertyAnimation::finished, this, &NewScheduleDialog::close);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        event->ignore();
    }
    else
        event->accept();
}

void NewScheduleDialog::showEvent(QShowEvent *event)
{
    event->accept();
    if(!event->spontaneous())
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
        animation->setStartValue(0);
        animation->setEndValue(1);
        animation->setDuration(300);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void NewScheduleDialog::onActionSaveTriggered()
{
    //当“保存”动作触发时，保存日程
    saveSchedule();
}

void NewScheduleDialog::saveSchedule()
{
    //获取日程标题
    QString titleStr = ui->lineEditTitle->text();
    //如果标题为空，弹出警告对话框
    if(titleStr.isEmpty() || titleStr.isNull())
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("标题不能为空，请填写标题！"));
        return;
    }

    //根据“新建日程”窗体中的内容修改日程指针schedule
    m_p_schedule->setTitle(titleStr);
    m_p_schedule->setTime(ui->dateTimeEditTime->dateTime(),m_is_first_save);
    m_p_schedule->setClock(ui->checkBoxClock->isChecked());
    m_p_schedule->setAddress(ui->plainTextEditAddress->toPlainText());
    m_p_schedule->setNote(ui->textEditNote->toHtml());

    //调用schedule的保存日程功能
    m_p_schedule->saveSchedule(m_is_first_save);

    //修改保存状态和第一次保存状态
    m_has_saved = true;
    m_is_first_save = false;

    //显示保存完毕对话框
    QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("保存完毕！"));

    //若设置了闹钟调用日程指针schedule中的添加闹钟功能；否则移除闹钟
    if(ui->checkBoxClock->isChecked())
    {
        m_p_schedule->addClock(ui->dateTimeEditTime->dateTime());
    }
    else
    {
        m_p_schedule->removeClock();
    }
}

void NewScheduleDialog::iniSignalsAndSlots()
{
    //当标题栏更改时修改m_has_saved
    connect(ui->lineEditTitle, &QLineEdit::textChanged, this, [=](const QString &) {
        m_has_saved = false;
    });

    //当时间栏更改时修改m_has_saved
    connect(ui->dateTimeEditTime, &QDateTimeEdit::dateTimeChanged, this, [=](const QDateTime &) {
        m_has_saved = false;
    });

    //当地址栏更改时修改m_has_saved
    connect(ui->plainTextEditAddress, &QPlainTextEdit::textChanged, this, [=]() {
        m_has_saved = false;
    });

    //当备注栏更改时修改m_has_saved
    connect(ui->textEditNote, &QTextEdit::textChanged, this, [=]() {
        m_has_saved = false;
    });

    //当保存事件触发时
    connect(ui->actionSave, &QAction::triggered, this, &NewScheduleDialog::onActionSaveTriggered);

    //当设置闹钟按键更改时修改m_has_saved
    connect(ui->checkBoxClock, &QCheckBox::stateChanged, this, [=](int state){
        m_has_saved = false;
        if(state == Qt::Checked)
        {
            qint64 timeLength = QDateTime::currentDateTime().msecsTo(ui->dateTimeEditTime->dateTime());
            if(timeLength <= 0)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("日程时间设置为过去的时间！"));
                ui->checkBoxClock->setChecked(false);
            }
            else if(timeLength <= 300000)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("请设置至少5分钟后的时间"));
                ui->checkBoxClock->setChecked(false);
            }
        }
    });

    //当字体下拉框更改时修改备注栏字体
    connect(ui->fontComboBox, &QFontComboBox::currentFontChanged, this, [=](const QFont &font){
        QFont f = ui->textEditNote->font();
        f.setFamily(font.family());
        ui->textEditNote->setFont(f);
    });

    //当字体大小数据框更改时修改备注栏字体大小
    connect(ui->spinBoxFontSize, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int size){
        QFont font = ui->textEditNote->font();
        font.setPointSize(size);
        ui->textEditNote->setFont(font);
    });

    // 当自定义日历组件的日期改变时修改DateTime输入组件的日期
    connect(m_custom_calendar_widget, &CustomCalendarWidget::signalSetCalendarTime, ui->dateTimeEditTime, &QDateTimeEdit::setDate);
}

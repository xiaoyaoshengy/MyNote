#include "customcalendarwidget.h"

#include <QProxyStyle>
#include <QLayout>
#include <QLocale>
#include <QTextCharFormat>
#include <QPainter>
#include <QPushButton>
#include <QLabel>

class CustomStyle : public QProxyStyle
{
public:
    CustomStyle(QWidget *parent)
    {
        setParent(parent);
    }

private:
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if(element == PE_FrameFocusRect)
        {
            return;
        }
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};

CustomCalendarWidget::CustomCalendarWidget(QWidget *parent) : QCalendarWidget(parent)
{
    initControl();
}

CustomCalendarWidget::~CustomCalendarWidget()
{

}

void CustomCalendarWidget::initControl()
{
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    setLocale(QLocale(QLocale::Chinese));
    setNavigationBarVisible(false);
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
    setStyle(new CustomStyle(this));

    QTextCharFormat format;
    format.setForeground(QColor(160, 160, 160));
    format.setBackground(QColor(255, 255, 255));

    setHeaderTextFormat(format);
    setWeekdayTextFormat(Qt::Saturday, format);
    setWeekdayTextFormat(Qt::Sunday, format);
    setWeekdayTextFormat(Qt::Monday, format);
    setWeekdayTextFormat(Qt::Tuesday, format);
    setWeekdayTextFormat(Qt::Wednesday, format);
    setWeekdayTextFormat(Qt::Thursday, format);
    setWeekdayTextFormat(Qt::Friday, format);

    initTopWidget();
    initBottomWidget();

    connect(this, &CustomCalendarWidget::currentPageChanged, this, &CustomCalendarWidget::setDataLabelTimeText);
}

void CustomCalendarWidget::initTopWidget()
{
    QWidget *topWidget = new QWidget(this);
    topWidget->setObjectName("CalendarTopWidget");
    topWidget->setFixedHeight(40);
    topWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->setContentsMargins(12, 0, 12, 0);
    hboxLayout->setSpacing(4);

    m_left_year_btn = new QPushButton(this);
    m_left_month_btn = new QPushButton(this);
    m_right_year_btn = new QPushButton(this);
    m_right_month_btn = new QPushButton(this);
    m_date_label = new QLabel(this);

    m_left_year_btn->setObjectName("CalendarLeftYearBtn");
    m_left_month_btn->setObjectName("CalendarLeftMonthBtn");
    m_right_year_btn->setObjectName("CalendarRightYearBtn");
    m_right_month_btn->setObjectName("CalendarRightMonthBtn");
    m_date_label->setObjectName("CalendarDataLabel");

    m_left_year_btn->setFixedSize(16, 16);
    m_left_month_btn->setFixedSize(16, 16);
    m_right_year_btn->setFixedSize(16, 16);
    m_right_month_btn->setFixedSize(16, 16);

    hboxLayout->addWidget(m_left_year_btn);
    hboxLayout->addWidget(m_left_month_btn);
    hboxLayout->addStretch();
    hboxLayout->addWidget(m_date_label);
    hboxLayout->addStretch();
    hboxLayout->addWidget(m_right_month_btn);
    hboxLayout->addWidget(m_right_year_btn);
    topWidget->setLayout(hboxLayout);

    QVBoxLayout *vBodyLayout = qobject_cast<QVBoxLayout*>(layout());
    vBodyLayout->insertWidget(0, topWidget);

    connect(m_left_year_btn, &QPushButton::clicked, this, &CustomCalendarWidget::onbtnClicked);
    connect(m_left_month_btn, &QPushButton::clicked, this, &CustomCalendarWidget::onbtnClicked);
    connect(m_right_year_btn, &QPushButton::clicked, this, &CustomCalendarWidget::onbtnClicked);
    connect(m_right_month_btn, &QPushButton::clicked, this, &CustomCalendarWidget::onbtnClicked);

    setDataLabelTimeText(selectedDate().year(), selectedDate().month());
}

void CustomCalendarWidget::initBottomWidget()
{
    QWidget *bottomWidget = new QWidget(this);
    bottomWidget->setObjectName("CalendarBottomWidget");
    bottomWidget->setFixedHeight(40);
    bottomWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->setContentsMargins(12, 0, 12, 0);
    hboxLayout->setSpacing(6);

    m_ensure_btn = new QPushButton(this);
    m_ensure_btn->setObjectName("CalendarEnsureBtn");
    m_ensure_btn->setFixedSize(40, 22);
    m_ensure_btn->setText(QString::fromLocal8Bit("确定"));

    m_today_btn = new QPushButton(this);
    m_today_btn->setObjectName("CalendarTodayBtn");
    m_today_btn->setFixedSize(40, 22);
    m_today_btn->setText(QString::fromLocal8Bit("现在"));

    hboxLayout->addStretch();
    hboxLayout->addWidget(m_today_btn);
    hboxLayout->addWidget(m_ensure_btn);
    bottomWidget->setLayout(hboxLayout);

    QVBoxLayout *vBodyLayout = qobject_cast<QVBoxLayout*>(layout());
    vBodyLayout->addWidget(bottomWidget);

    connect(m_ensure_btn, &QPushButton::clicked, this, [=](){
        emit signalSetCalendarTime(selectedDate());
        emit activated(selectedDate());
    });
    connect(m_today_btn, &QPushButton::clicked, this, &CustomCalendarWidget::showToday);
}

void CustomCalendarWidget::onbtnClicked()
{
    QPushButton *senderBtn = qobject_cast<QPushButton*>(sender());
    if(senderBtn == m_left_year_btn)
    {
        showPreviousYear();
    }
    else if(senderBtn == m_left_month_btn)
    {
        showPreviousMonth();
    }
    else if(senderBtn == m_right_year_btn)
    {
        showNextYear();
    }
    else if(senderBtn == m_right_month_btn)
    {
        showNextMonth();
    }
}

void CustomCalendarWidget::setDataLabelTimeText(int year, int month)
{
    m_date_label->setText(QString::number(year) + QString::fromLocal8Bit("年") + QString::number(month) + QString::fromLocal8Bit("月"));
}

void CustomCalendarWidget::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    if(date == selectedDate())
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0, 145, 255));

        painter->drawRoundedRect(rect.x(), rect.y() + 3, rect.width(), rect.height() - 6, 3, 3);
        painter->setPen(QColor(255, 255, 255));

        painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));
        painter->restore();
    }
    else if(date == QDate::currentDate())
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0, 161, 255));
        painter->drawRoundedRect(rect.x(), rect.y() + 3, rect.width(), rect.height() - 6, 3, 3);
        painter->setBrush(QColor(255, 255, 255));
        painter->drawRoundedRect(rect.x() + 1, rect.y() + 4, rect.width() - 2, rect.height() - 8, 2, 2);
        painter->setPen(QColor(0, 161, 255));

        painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));
        painter->restore();
    }
    else if(date < minimumDate() || date > maximumDate())
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(249, 249, 249));

        painter->drawRect(rect.x(), rect.y() + 3, rect.width(), rect.height() - 6);
        painter->setPen(QColor(220, 220, 220));

        painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));
        painter->restore();
    }
    else
    {
        QCalendarWidget::paintCell(painter, rect, date);
    }
}

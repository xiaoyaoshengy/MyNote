#include "calendarlabel.h"

#include <QPainter>
#include <QMouseEvent>

CalendarLabel::CalendarLabel(QWidget *parent) : QLabel(parent)
{
    m_date = QDate::currentDate();
    m_selected_date = m_date;
    resize(340, 240);
    connect(this, &CalendarLabel::dateChanged, this, &CalendarLabel::onDateChanged);
    connect(this, &CalendarLabel::selectedDateChanged, this, &CalendarLabel::onSelectedDateChanged);
}

void CalendarLabel::setDate(QDate date)
{
    m_date = date;
    emit dateChanged(date);
}

void CalendarLabel::setSelectedDate(QDate date)
{
    m_selected_date = date;
    emit selectedDateChanged(date);
}

QDate CalendarLabel::selectedDate()
{
    return m_selected_date;
}

void CalendarLabel::paintEvent(QPaintEvent *event)
{
    // 根据显示日期绘制该月的日历界面
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::TextAntialiasing);
    QFont font;
    font.setFamily("Cambria");
    font.setPointSize(20);
    painter.setFont(font);

    int days = m_date.daysInMonth();
    QDate firstDay = QDate(m_date.year(), m_date.month(), 1);
    int firstDayOfWeek = firstDay.dayOfWeek() % 7;
    int x = firstDayOfWeek * 50;
    for(int i = 0; i < days; i++)
    {
        int y = (firstDayOfWeek + i) / 7 * 40;
        painter.save();
        painter.setPen(Qt::NoPen);
        painter.setRenderHint(QPainter::Antialiasing);
        if((i + 1) == m_selected_date.day() && m_selected_date.year() == m_date.year() && m_selected_date.month() == m_date.month())
        {
            painter.fillRect(QRect(x, y, 40, 40), QBrush(QColor(20, 133, 251)));
        }
        else
        {
            painter.fillRect(QRect(x, y, 40, 40), QBrush(Qt::white));
        }
        QDate curDate = QDate::currentDate();
        if((i + 1) == curDate.day() && curDate.year() == m_date.year() && curDate.month() == m_date.month())
        {
            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            QPixmap pix(QString(":/img/images/circle.png"));
            painter.drawPixmap(QRect(x, y, 40, 40), pix);
        }
        painter.restore();
        if((i + 1) == m_selected_date.day() && m_selected_date.year() == m_date.year() && m_selected_date.month() == m_date.month())
        {
            painter.save();
            painter.setPen(Qt::white);
            painter.drawText(QRect(x, y, 40, 40), Qt::AlignCenter, QString::number(i + 1));
            painter.restore();
        }
        else
        {
            painter.drawText(QRect(x, y, 40, 40), Qt::AlignCenter, QString::number(i + 1));
        }
        x = (x + 50) % 350;
    }
}

void CalendarLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;

    QPoint pos = event->pos();
    int selectedX = pos.x() / 50 * 50;
    int selectedY = pos.y() / 40 * 40;
    QDate firstDay = QDate(m_date.year(), m_date.month(), 1);
    int firstDayOfWeek = firstDay.dayOfWeek() % 7;
    int x = firstDayOfWeek * 50;
    for(int i = 0; i < m_date.daysInMonth(); i++)
    {
        int y = (firstDayOfWeek + i) / 7 * 40;
        if(x == selectedX && y == selectedY)
        {
            setSelectedDate(QDate(m_date.year(), m_date.month(), i + 1));
            emit doubleClicked(QDate(m_date.year(), m_date.month(), i + 1));
            return;
        }
        x = (x + 50) % 350;
    }
}

void CalendarLabel::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;

    QPoint pos = event->pos();
    int selectedX = pos.x() / 50 * 50;
    int selectedY = pos.y() / 40 * 40;
    QDate firstDay = QDate(m_date.year(), m_date.month(), 1);
    int firstDayOfWeek = firstDay.dayOfWeek() % 7;
    int x = firstDayOfWeek * 50;
    for(int i = 0; i < m_date.daysInMonth(); i++)
    {
        int y = (firstDayOfWeek + i) / 7 * 40;
        if(x == selectedX && y == selectedY)
        {
            setSelectedDate(QDate(m_date.year(), m_date.month(), i + 1));
            return;
        }
        x = (x + 50) % 350;
    }
}

void CalendarLabel::onDateChanged(QDate date)
{
    Q_UNUSED(date);
    update();
}

void CalendarLabel::onSelectedDateChanged(QDate date)
{
    Q_UNUSED(date);
    update();
}

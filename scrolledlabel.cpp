#include "scrolledlabel.h"

#include <QResizeEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTimer>

ScrolledLabel::ScrolledLabel(QWidget *parent) : QLabel(parent)
{
    m_horizon_num = height();
    m_turn_next = true;
    m_p_pixmaps = nullptr;
    m_pixmaps_length = 0;
    m_pixmap_num = 0;
    connect(this, &ScrolledLabel::horizonNumChanged, this, &ScrolledLabel::onHorizonNumChanged);
}

int ScrolledLabel::horizonNum()
{
    return m_horizon_num;
}

void ScrolledLabel::setHorizonNum(int hnum)
{
    m_horizon_num = hnum;
    emit horizonNumChanged(hnum);
}

void ScrolledLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if(!m_current_pixmap.isNull() && !m_next_pixmap.isNull() && !m_previous_pixmap.isNull())
    {
        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::NoBrush);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        if(m_turn_next)
        {
            if(m_horizon_num >= m_current_pixmap.height() / 2)
            {
                QRect target(0, 0, m_current_pixmap.width(), m_current_pixmap.height() / 2);
                QRect source(0, 0, m_current_pixmap.width(), m_current_pixmap.height() / 2);
                painter.drawPixmap(target, m_current_pixmap, source);
                target.setRect(0, m_current_pixmap.height() / 2, m_current_pixmap.width(), m_horizon_num - m_current_pixmap.height() / 2);
                source.setRect(0, m_current_pixmap.height() / 2, m_current_pixmap.width(), m_current_pixmap.height() / 2);
                painter.drawPixmap(target, m_current_pixmap, source);
                target.setRect(0, m_horizon_num, m_next_pixmap.width(), m_next_pixmap.height() - m_horizon_num);
                source.setRect(0, m_horizon_num, m_next_pixmap.width(), m_next_pixmap.height() - m_horizon_num);
                painter.drawPixmap(target, m_next_pixmap, source);
            }
            else
            {
                QRect target(0, 0, m_current_pixmap.width(), m_horizon_num);
                QRect source(0, 0, m_current_pixmap.width(), m_horizon_num);
                painter.drawPixmap(target, m_current_pixmap, source);
                target.setRect(0, m_horizon_num, m_next_pixmap.width(), m_next_pixmap.height() / 2 - m_horizon_num);
                source.setRect(0, 0, m_next_pixmap.width(), m_next_pixmap.height() / 2);
                painter.drawPixmap(target, m_next_pixmap, source);
                target.setRect(0, m_next_pixmap.height() / 2, m_next_pixmap.width(), m_next_pixmap.height() / 2);
                source.setRect(0, m_next_pixmap.height() / 2, m_next_pixmap.width(), m_next_pixmap.height() / 2);
                painter.drawPixmap(target, m_next_pixmap, source);
            }
        }
        else
        {
            if(m_horizon_num <= m_current_pixmap.height() / 2)
            {
                QRect target(0, 0, m_previous_pixmap.width(), m_horizon_num);
                QRect source(0, 0, m_previous_pixmap.width(), m_horizon_num);
                painter.drawPixmap(target, m_previous_pixmap, source);
                target.setRect(0, m_horizon_num, m_current_pixmap.width(), m_current_pixmap.height() / 2 - m_horizon_num);
                source.setRect(0, 0, m_current_pixmap.width(), m_current_pixmap.height() / 2);
                painter.drawPixmap(target, m_current_pixmap, source);
                target.setRect(0, m_current_pixmap.height() / 2, m_current_pixmap.width(), m_current_pixmap.height() / 2);
                source.setRect(0, m_current_pixmap.height() / 2, m_current_pixmap.width(), m_current_pixmap.height() / 2);
                painter.drawPixmap(target, m_current_pixmap, source);
            }
            else
            {
                QRect target(0, 0, m_previous_pixmap.width(), m_previous_pixmap.height() / 2);
                QRect source(0, 0, m_previous_pixmap.width(), m_previous_pixmap.height() / 2);
                painter.drawPixmap(target, m_previous_pixmap, source);
                target.setRect(0, m_previous_pixmap.height() / 2, m_previous_pixmap.width(), m_horizon_num - m_previous_pixmap.height() / 2);
                source.setRect(0, m_previous_pixmap.height() / 2, m_previous_pixmap.width(), m_previous_pixmap.height() / 2);
                painter.drawPixmap(target, m_previous_pixmap, source);
                target.setRect(0, m_horizon_num, m_current_pixmap.width(), m_current_pixmap.height() - m_horizon_num);
                source.setRect(0, m_horizon_num, m_current_pixmap.width(), m_current_pixmap.height() - m_horizon_num);
                painter.drawPixmap(target, m_current_pixmap, source);
            }
        }
    }
}

void ScrolledLabel::resizeEvent(QResizeEvent *event)
{
    m_horizon_num = event->size().height();
    QLabel::resizeEvent(event);
}

void ScrolledLabel::setPixmaps(QPixmap *pixmaps, int length)
{
    m_p_pixmaps = pixmaps;
    m_pixmaps_length = length;
}

void ScrolledLabel::setPixmapNum(int num)
{
    num = (num + m_pixmaps_length) % m_pixmaps_length;
    emit pixmapNumChanged(num);
    m_pixmap_num = num;
    if(m_p_pixmaps)
    {
        m_current_pixmap = m_p_pixmaps[num % m_pixmaps_length];
        m_previous_pixmap = m_p_pixmaps[(num - 1 + m_pixmaps_length) % m_pixmaps_length];
        m_next_pixmap = m_p_pixmaps[(num + 1) % m_pixmaps_length];
    }
}

void ScrolledLabel::setTurnDirection(bool turnNext)
{
    m_turn_next = turnNext;
}

void ScrolledLabel::wheelEvent(QWheelEvent *event)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "horizonNum");
    if(event->angleDelta().y() < 0)
    {
        m_turn_next = true;
        animation->setStartValue(height());
        animation->setEndValue(0);
    }
    else
    {
        m_turn_next = false;
        animation->setStartValue(0);
        animation->setEndValue(height());
    }
    animation->setEasingCurve(QEasingCurve::InOutSine);
    animation->setDuration(500);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    QTimer::singleShot(525, this, [=](){
        if(m_turn_next)
            setPixmapNum(m_pixmap_num + 1);
        else
            setPixmapNum(m_pixmap_num - 1);
    });
}

void ScrolledLabel::onHorizonNumChanged(int hnum)
{
    Q_UNUSED(hnum);
    update();
}

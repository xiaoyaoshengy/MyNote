#ifndef CUSTOMCALENDARWIDGET_H
#define CUSTOMCALENDARWIDGET_H

#include <QCalendarWidget>

class QPushButton;
class QLabel;

/**
 * @brief The CustomCalendarWidget class
 * 自定义日历组件类，主要用于替换DateTime输入组件中的原生日历组件
 */
class CustomCalendarWidget : public QCalendarWidget
{
    Q_OBJECT
public:
    /**
     * @brief CustomCalendarWidget 构造函数
     * @param parent 父窗体指针
     */
    CustomCalendarWidget(QWidget *parent);

    /**
     * @brief ~CustomCalendarWidget 析构函数
     */
    ~CustomCalendarWidget();

private:
    /**
     * @brief initControl
     * 初始化控制组件
     */
    void initControl();

    /**
     * @brief initTopWidget
     * 初始化顶部组件
     */
    void initTopWidget();

    /**
     * @brief initBottomWidget
     * 初始化底部组件
     */
    void initBottomWidget();

signals:
    /**
     * @brief signalSetCalendarTime 日历时间改变信号
     * @param data 新的时间
     */
    void signalSetCalendarTime(const QDate& data);

private slots:
    /**
     * @brief onbtnClicked
     * 当按钮点击时的处理函数
     */
    void onbtnClicked();

    /**
     * @brief setDataLabelTimeText
     * 设置显示日期的标签内容
     * @param year 显示年份
     * @param month 显示月份
     */
    void setDataLabelTimeText(int year, int month);

protected:
    /**
     * @brief paintCell 绘制单元格
     * @param painter 画笔
     * @param rect 单元格
     * @param date 日期
     */
    void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;

private:
    /**
     * @brief m_left_year_btn “上一年”按钮
     */
    QPushButton *m_left_year_btn;

    /**
     * @brief m_left_month_btn “上一月”按钮
     */
    QPushButton *m_left_month_btn;

    /**
     * @brief m_right_year_btn “下一年”按钮
     */
    QPushButton *m_right_year_btn;

    /**
     * @brief m_right_month_btn “下一月”按钮
     */
    QPushButton *m_right_month_btn;

    /**
     * @brief m_ensure_btn “确定”按钮
     */
    QPushButton *m_ensure_btn;

    /**
     * @brief m_today_btn “现在”按钮
     */
    QPushButton *m_today_btn;

    /**
     * @brief m_date_label 显示日期的标签
     */
    QLabel *m_date_label;
};

#endif // CUSTOMCALENDARWIDGET_H

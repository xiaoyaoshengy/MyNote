#ifndef CALENDARLABEL_H
#define CALENDARLABEL_H

#include <QLabel>
#include <QDate>

/**
 * @brief The CalendarLabel class
 * 自定义的日历标签类
 */
class CalendarLabel : public QLabel
{
    Q_OBJECT
public:
    /**
     * @brief CalendarLabel 构造函数
     * @param parent 父窗体指针
     */
    CalendarLabel(QWidget *parent = nullptr);

    /**
     * @brief setDate 设置显示日期
     * 设置该自定义日历标签类应该显示的日期
     * @param date 显示日期
     */
    void setDate(QDate date);

    /**
     * @brief setSelectedDate 设置选中日期
     * 设置该自定义日历标签类的选中日期
     * @param date 选中日期
     */
    void setSelectedDate(QDate date);

    /**
     * @brief selectedDate 获取选中日期
     * @return 选中日期
     */
    QDate selectedDate();

    /**
     * @brief paintEvent 绘制事件处理函数
     * 重写父类同名函数，自定义处理绘制事件
     * @param event 绘制事件
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief mouseDoubleClickEvent 鼠标双击事件处理函数
     * 重写父类同名函数，自定义处理鼠标双击事件
     * @param event 鼠标双击事件
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    /**
     * @brief mousePressEvent 鼠标按下事件处理函数
     * 重写父类同名函数，自定义处理鼠标按下事件
     * @param event 鼠标按下事件
     */
    void mousePressEvent(QMouseEvent *event) override;

signals:
    /**
     * @brief dateChanged 显示日期改变信号
     * 当显示日期改变时发射
     * @param date 新的显示日期
     */
    void dateChanged(QDate date);

    /**
     * @brief selectedDateChanged 选中日期改变信号
     * 当选中日期改变时发射
     * @param selectDate 新的选中日期
     */
    void selectedDateChanged(QDate selectDate);

    /**
     * @brief doubleClicked 鼠标双击信号
     * 当鼠标双击事件产生时发射
     * @param date 鼠标双击事件产生时鼠标选中的日期
     */
    void doubleClicked(QDate date);

private slots:
    /**
     * @brief onDateChanged
     * 当显示日期改变时的处理函数
     * @param date 新的显示日期
     */
    void onDateChanged(QDate date);

    /**
     * @brief onSelectedDateChanged
     * 当选中日期改变时的处理函数
     * @param date 新的选中日期
     */
    void onSelectedDateChanged(QDate date);

private:
    /**
     * @brief m_date 显示日期
     */
    QDate m_date;

    /**
     * @brief m_selected_date 选中日期
     */
    QDate m_selected_date;
};

#endif // CALENDARLABEL_H

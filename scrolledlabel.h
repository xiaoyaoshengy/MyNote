#ifndef SCROLLEDLABEL_H
#define SCROLLEDLABEL_H

#include <QLabel>

class ScrolledLabel : public QLabel
{
    Q_OBJECT
    //翻页属性
    Q_PROPERTY(int horizonNum READ horizonNum WRITE setHorizonNum NOTIFY horizonNumChanged)
public:
    ScrolledLabel(QWidget *parent = nullptr);

    //定义翻页属性相关函数
    int horizonNum();
    void setHorizonNum(int hnum);

    //重写绘制事件
    void paintEvent(QPaintEvent *event) override;

    //重写大小改变事件
    void resizeEvent(QResizeEvent *event) override;

    //设置图片序列
    void setPixmaps(QPixmap *pixmaps, int length);

    //设置当前图片序号
    void setPixmapNum(int num);

    //设置翻页方向
    void setTurnDirection(bool turnNext);

    //重写滚轮事件
    void wheelEvent(QWheelEvent *event) override;

signals:
    void horizonNumChanged(int hnum);
    void pixmapNumChanged(int newNum);

private slots:
    void onHorizonNumChanged(int hnum);

private:
    //翻页属性
    int m_horizon_num;
    //当前图片
    QPixmap m_current_pixmap;
    //下一张图片
    QPixmap m_next_pixmap;
    //上一张图片
    QPixmap m_previous_pixmap;
    //翻页方向
    bool m_turn_next;
    //图片序列
    QPixmap *m_p_pixmaps;
    //当前图片序号
    int m_pixmap_num;
    //图片序列长度
    int m_pixmaps_length;

};

#endif // SCROLLEDLABEL_H

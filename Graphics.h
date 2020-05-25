#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QDebug>



class GraphicsView : public QGraphicsView
{
    using QGraphicsView::QGraphicsView;
public:
    QPointF pos_mouse{};

    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    //滚轮事件
    void wheelEvent(QWheelEvent *event) override;


//    void paintEvent(QPaintEvent *event) override;

//    void drawForeground(QPainter *painter, const QRectF &rect) override;

};


class GraphicsScene : public QGraphicsScene
{
    using QGraphicsScene::QGraphicsScene;
public:
    QBrush brush_internal{};//内部笔刷
    QBrush brush_external{};//外部笔刷

    QRectF rect_exposed{};//暴露的矩形

public:
    //绘制背景
    void drawBackground(QPainter *painter, const QRectF &rect) override;


};

//背景图对象
//坑爹得一点是, 多继承要把QObject放在前面(-_-)||
class GraphicsPixmapItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity FINAL)//注册一下属性(这个注册真的好难写啊)
    using QGraphicsPixmapItem::QGraphicsPixmapItem;//使用基类全部构造函数

};


//QGraphicsObject;


#endif // GRAPHICSSCENE_H

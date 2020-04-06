#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsView>
#include <QGraphicsScene>
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

};


class GraphicsScene : public QGraphicsScene
{
    using QGraphicsScene::QGraphicsScene;
public:
    QBrush brush_internal{};//内部笔刷
    QBrush brush_external{};//外部笔刷

public:
    //绘制背景
    void drawBackground(QPainter *painter, const QRectF &rect) override;
};





#endif // GRAPHICSSCENE_H

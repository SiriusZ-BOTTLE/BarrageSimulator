#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QDebug>

#include "Core.h"


class GraphicsView : public QGraphicsView
{
    using QGraphicsView::QGraphicsView;
public:
    QPoint pos_mouse{};

    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

};


class GraphicsScene : public QGraphicsScene
{
    using QGraphicsScene::QGraphicsScene;

public:
    //绘制背景
    void drawBackground(QPainter *painter, const QRectF &rect) override;
};





#endif // GRAPHICSSCENE_H

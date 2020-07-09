#include "universal.h"


void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
    pos_mouse=this->mapToScene(event->pos());
//    qDebug()<<pos_mouse;
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
//    qDebug()<<"press";
    event->ignore();
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
//    qDebug()<<"release";
    event->ignore();
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    event->accept();//忽略事件
}


///绘制背景
void GraphicsScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    rect_exposed=rect;
//    qDebug()<<rect_exposed;
    static int count;
    const auto& rect_scene=this->sceneRect();

    if(rect_scene.contains(rect))//包含
    {
        painter->setBrush(brush_internal);
        painter->drawRect(rect);
    }
    else
    {
        QRectF intersection=rect_scene.intersected(rect);//获取交集部分

        painter->setBrush(brush_external);
        painter->drawRect(rect);

        painter->setBrush(brush_internal);
        painter->drawRect(intersection);
    }

//    QBrush brush;
//    brush.setColor(QColor(0xcc,0xcc,0xcc));
//    painter->drawRect(rect_scene);


}










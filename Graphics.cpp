#include "universal.h"


void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
    pos_mouse=event->pos();
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<"press";
    event->ignore();
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug()<<"release";
    event->ignore();
}


///绘制背景
void GraphicsScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    static int count;
    painter->setBrush(QBrush(QColor(0xff,0xff,0xff)));
    painter->drawRect(rect);
//    using namespace Core;

//    static int t=0;


//    if(t==0)
//    {
//        Decimal rand=ToolFunctions::get_random_decimal_0_1();//获取一个随机数,用于处理颜色

//        int value = 180+50*rand;//获得区间范围内的值

//        painter->setBrush(QBrush(QColor(value,value,value)));
//        painter->drawRect(rect);



//        t=100;
//    }

//    --t;


}

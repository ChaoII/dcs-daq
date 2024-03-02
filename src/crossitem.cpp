#include "crossitem.h"

CrossItem::CrossItem()
{

}

QRectF CrossItem::boundingRect() const
{

    return QRect(0,0,100,100);
}

void CrossItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    painter->drawLine(QLine(QPoint(50,50),QPoint(300,300)));

}

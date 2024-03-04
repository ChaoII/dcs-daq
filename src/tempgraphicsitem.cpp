#include "tempgraphicsitem.h"


TempGraphicsItem::TempGraphicsItem(const QSizeF &size):
    size_(size)
{
    setCacheMode(DeviceCoordinateCache);
    temp_canvas = new QImage(size_.toSize(),QImage::Format_ARGB32_Premultiplied);
    temp_canvas->fill(Qt::transparent);
    temp_painter = new QPainter(temp_canvas);
}

TempGraphicsItem::~TempGraphicsItem()
{

}

void TempGraphicsItem::clear()
{
    temp_canvas->fill(Qt::transparent);
    this->update();
}

QRectF TempGraphicsItem::boundingRect() const
{
    return  QRectF(0,0,size_.width(),size_.height());
}

void TempGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(0,0,*temp_canvas);
}

void TempGraphicsItem::draw_shape(const QPointF &p1, const QPointF &p2, const QColor &color, int line_width)
{
    temp_canvas->fill(Qt::transparent);
    temp_painter->setRenderHint(QPainter::Antialiasing, true);
    temp_painter->setCompositionMode(QPainter::CompositionMode_Source);
    temp_painter->setPen(QPen(color,line_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    temp_painter->drawRect(QRectF(p1,p2));
    update();
}



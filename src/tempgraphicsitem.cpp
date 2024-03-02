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
//    SAFE_DELETE(m_pTempPainter);
//    SAFE_DELETE(m_pTempCanvas);
//    qDebug() << "--->>>Lynn<<<---" << __FUNCTION__;
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

void TempGraphicsItem::draw_shape(QPointF p1, QPointF p2)
{
    temp_canvas->fill(Qt::transparent);
    temp_painter->setRenderHint(QPainter::Antialiasing, true);
    temp_painter->setCompositionMode(QPainter::CompositionMode_Source);
    temp_painter->setPen(QPen(Qt::red,3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    temp_painter->drawRect(QRectF(p1,p2));
    update();
}

void TempGraphicsItem::set_size(const QSize &size)
{
//    size_ = size;
//    *temp_canvas = temp_canvas->scaled(size);
}


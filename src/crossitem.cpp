#include "crossitem.h"

CrossItem::CrossItem() {

}

QRectF CrossItem::boundingRect() const {

    return QRect(0, 0, 100, 100);
}

void CrossItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    //绘制横向线
    painter->drawLine(-3000, point_.y(), view_port_width_, point_.y());
    //绘制纵向线
    painter->drawLine(point_.x(), -3000, point_.x(), view_port_height_);

}

void CrossItem::draw_shape(const QPoint &point, int w, int h) {

    point_ = point;
    view_port_width_ = w;
    view_port_height_ = h;
}

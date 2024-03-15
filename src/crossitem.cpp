#include "crossitem.h"
#include "utils/config.h"

CrossItem::CrossItem() {

}

QRectF CrossItem::boundingRect() const {
    return QRect(0, 0, Config::Instance().default_scene_size, Config::Instance().default_scene_size);
}

void CrossItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::red);
    painter->setPen(pen);
    //绘制横向线
    painter->drawLine(left_top_point_.x(), cursor_point_.y(), right_bottom_point_.x(), cursor_point_.y());
    //绘制纵向线
    painter->drawLine(cursor_point_.x(), left_top_point_.y(), cursor_point_.x(), right_bottom_point_.y());
}

void CrossItem::draw_shape(const QPoint &cursor_point, const QPoint &left_top_point, const QPoint &right_bottom_point) {
    prepareGeometryChange();
    cursor_point_ = cursor_point;
    left_top_point_ = left_top_point;
    right_bottom_point_ = right_bottom_point;
    update();
}

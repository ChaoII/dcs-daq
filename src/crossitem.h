
#pragma once

#include <QtWidgets>

class CrossItem : public QGraphicsItem {

public:
    CrossItem();

    void draw_shape(const QPoint &cursor_point, const QPoint &left_top_point, const QPoint &right_bottom_point) ;

protected:
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    QPoint cursor_point_;
    QPoint left_top_point_;
    QPoint right_bottom_point_;
};


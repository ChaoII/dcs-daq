
#pragma once
#include <QtWidgets>

class CrossItem : public QGraphicsItem {

public:
    CrossItem();

    void draw_shape(const QPoint &point, int w, int h);

protected:
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;


private:
    QPoint point_;
    int view_port_width_{};
    int view_port_height_{};
};


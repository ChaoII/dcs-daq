

#include <QtWidgets>

class CrossItem : public QGraphicsItem {

public:
    CrossItem();

protected:
    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
};


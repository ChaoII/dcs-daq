

#include <QGraphicsItem>
#include <QPainter>

class TempGraphicsItem : public QGraphicsItem {
public:
    explicit TempGraphicsItem(const QSizeF &size);

    ~TempGraphicsItem();

    void clear();

    void draw_shape(const QPointF &p1, const QPointF &p2, const QColor &color = Qt::red, int line_width = 1);

protected:

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QSizeF size_;
    QImage *temp_canvas = nullptr;
    QPainter *temp_painter = nullptr;

};


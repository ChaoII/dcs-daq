
#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include "tempgraphicsitem.h"
#include "crossitem.h"


namespace Ui {
    class AGraphicsView;
}

class AGraphicsView : public QGraphicsView {
Q_OBJECT

public:
    explicit AGraphicsView(QWidget *parent = nullptr);

    ~AGraphicsView();

    void add_image_item(const QPixmap &pix);

    void set_draw_shape_status(bool checked_status);

protected:

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

signals:

    void send_position_signal(const QPoint &, const QPoint &);

    void send_draw_final_signal(const QGraphicsRectItem *item);

private:
    int index_id = 0;
    Ui::AGraphicsView *ui;
    QPoint last_point_;
    QPoint current_point_;
    TempGraphicsItem *temp_canvas_ = nullptr;
    bool draw_rect_checked_ = false;
    CrossItem *cross_item_ = nullptr;
    double scale_factor = 1.0;
};


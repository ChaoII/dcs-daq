
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

    struct ScaleRange {
        double minimum = 0;
        double maximum = 0;
    };

public:
    explicit AGraphicsView(QWidget *parent = nullptr);

    ~AGraphicsView();

    void add_image_item(const QPixmap &pix);

    void set_draw_shape_status(bool checked_status);

    void scale_down();

    void scale_up();

    void setup_scale(double scale);

    void set_scale_range(double minimum, double maximum);

private:
    void center_scene();

protected:

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    void showEvent(QShowEvent *event) override;

    void drawBackground(QPainter *painter, const QRectF &r) override;

signals:

    void send_position_signal(const QPoint &, const QPoint &);

    void send_draw_final_signal(const QGraphicsRectItem *item);

private:
    QColor box_color = QColor(255, 0, 0, 30);

    Ui::AGraphicsView *ui;

    QPoint last_point_;

    QPoint current_point_;

    TempGraphicsItem *temp_canvas_ = nullptr;

    bool draw_rect_checked_ = false;

    CrossItem *cross_item_ = nullptr;

    ScaleRange scale_range_ = {};

    int default_scene_size_ = 0;
};


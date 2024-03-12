
#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include "tempgraphicsitem.h"
#include "crossitem.h"
#include "arectitem.h"
#include "config.h"

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

    ~AGraphicsView() override;

    void add_image_item(const QPixmap &pix);

    void set_color(const QColor &color);

    void set_draw_shape_status();

    void set_select_status();

    void scale_down();

    void scale_up();

    void setup_scale(double scale);

    void set_scale_range(double minimum, double maximum);

    void remove_item_from_scene(QGraphicsItem *);

    void update_background_image(const QImage &img);

    void draw_real_rect(const QString &id, const QRectF &rect);

private:
    void center_scene();

    void hide_cross_line();

    void show_cross_line();

protected:

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    void showEvent(QShowEvent *event) override;

    void drawBackground(QPainter *painter, const QRectF &r) override;

signals:

    void send_position_signal(const QPoint &, const QPoint &);

    void send_draw_final_signal(ARectItem *);

    void item_selected_changed_signal();

    void item_changed_signal(ARectItem *);

    void update_image_signal(const QImage &);

private slots:

    void on_mouse_is_enter_item(bool is_hover);

    void on_item_changed();



private:

    Ui::AGraphicsView *ui;
    /// 绘制矩形框的颜色
    QColor box_color_;
    /// 在鼠标移动前所点击的点
    QPoint last_point_;
    /// 鼠标点击的当前点
    QPoint current_point_;
    /// 临时画布，用于双缓冲绘图
    TempGraphicsItem *temp_canvas_ = nullptr;
    /// 鼠标所在位置的十字线
    CrossItem *cross_item_ = nullptr;
    /// 缩放比例
    ScaleRange scale_range_ = {};
    /// 设置默认场景尺寸
    int default_scene_size_ = 0;
    /// 绘制矩形按钮被选中
    bool draw_rect_checked_ = false;
    /// 注意，绘制矩形按钮被选中后，不一定能绘图，因为如果鼠标点在另一个可移动图元上进行绘图，会拖动当前图元的同时绘图，所以仅用了此行为
    bool can_draw_ = true;
    /// 是否加载背景
    bool is_load_background_picture_ = false;
    /// 当当前鼠标所在位置的图元数大于items_threshold_时禁用绘制行为，即设置can_draw_为false
    int items_threshold_ = 2;
    QGraphicsPixmapItem *background_img_item = nullptr;
};


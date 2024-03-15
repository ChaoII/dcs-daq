//
// Created by AC on 2024/3/7.
//

#pragma once

#include <QGraphicsObject>
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneEvent>
#include <QKeyEvent>
#include <QGraphicsScene>


class ARectItem : public QGraphicsObject {

    /// item的操作类型
    enum ItemOperation {
        /// 无操作
        NONE,
        /// 移动操作
        MOVE,
        /// 缩放操作
        RESIZE,
    };
    /// 鼠标类型
    enum CursorType {
        ARROW_CURSOR = 0x00,
        RESIZE_LEFT_TOP_CURSOR = 0x01,
        RESIZE_RIGHT_TOP_CURSOR,
        RESIZE_LEFT_BOTTOM_CURSOR,
        RESIZE_RIGHT_BOTTOM_CURSOR,
        HAND_OPEN_CURSOR,
        HAND_CLOSE_CURSOR,
    };

Q_OBJECT
public:
    explicit ARectItem(QString tag_id,QString  tag_name, const QRectF &rect);

    /// 设置item tag_id
    /// \param name item tag_id
    void set_tag_id(const QString &tag_id);

    /// 获取item tag_id
    /// \return
    QString get_tag_id();

    /// 设置item名称
    /// \param name item名称
    void set_tag_name(const QString &name);

    /// 获取item名称
    /// \return
    QString get_tag_name();

    /// 获取item的id
    /// \return
    QString get_id();

    /// 设置item的颜色
    /// \param color 颜色
    void set_color(const QColor &color);

    /// 更新item的参数
    void update_item();

    QRectF get_inner_rect();

    void move_by(const QPointF &point);

private:
    /// 根据角度计算鼠标光标的类型
    /// \param angle 角度
    /// \return 鼠标光标类型
    Qt::CursorShape get_resize_cursor_shape(qreal angle);

    static void resize_and_check_rect(const QPointF &p1, const QPointF &p2, QRectF &rect);


protected:

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

signals:

    void item_changed_signal();

    void mouse_hover_signal(bool is_hover);

private:
    /// 绘制矩形框的颜色
    QColor color_;
    /// 矩形框范围
    QRectF rect_;
    /// item id
    QString tag_id_;
    /// item 名称
    QString tag_name_;
    /// item id
    QString id_;
    /// item 选中时外边界虚线框
    QRectF dash_box_;
    /// item 的左上角
    QPointF left_top_;
    /// 鼠标类型
    CursorType cursor_type_ = CursorType::ARROW_CURSOR;
    /// 鼠标操作
    ItemOperation operation_ = ItemOperation::NONE;
    /// 鼠标按下时记录的点位
    QPointF press_pos_;
    /// item的横纵比
    qreal item_ratio_;
    /// 外边界框矩形距Item的pad
    int pad_size_ = 10;
    /// 四角缩放控制点图像直径
    int control_point_size_ = 6;
    /// 旋转点距离外框矩形的距离
    int ratio_line_len_ = 12;
    /// //旋转点图像直径
    int rotate_ellipse_width_ = 10;
    /// item选中时四个缩放控制点和旋转控制点的绘制位置
    QRectF top_left_rect_, top_right_rect_, bottom_left_rect_, bottom_right_rect_, rotate_rect_;
    /// 各顶点的角度
    qreal top_left_angle_, top_right_angle_, bottom_left_angle_, bottom_right_angle_;
    /// 鼠标悬浮在旋转控制点时的光标样式
    QCursor rotate_hover_cursor_;
    /// 鼠标点击拖动旋转控制点时的光标样式
    QCursor rotate_press_cursor_;
    /// item的变换矩阵
    QTransform transform_;
};




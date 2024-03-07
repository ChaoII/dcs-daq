//
// Created by AC on 2024/3/7.
//

#pragma once

#include <QGraphicsObject>
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneEvent>
#include <QKeyEvent>


class ARectItem : public QGraphicsObject {

    enum ItemOperation {
        NONE,
        MOVE,
        RESIZE,
        ROTATE
    };
    enum CursorType {
        ARROW_CURSOR = 0x00,
        RESIZE_LEFT_TOP_CURSOR = 0x01,
        RESIZE_RIGHT_TOP_CURSOR,
        RESIZE_LEFT_BOTTOM_CURSOR,
        RESIZE_RIGHT_BOTTOM_CURSOR,
        HAND_OPEN_CURSOR,
        HAND_CLOSE_CURSOR,
        ROTATE_CURSOR,
    };

Q_OBJECT
public:
    explicit ARectItem(const QRectF &rect);

    void set_name(const QString &name);

    QString get_name();

    QString get_id();

    void set_color(const QColor &color);

    void item_resize();

private:
    Qt::CursorShape GetResizeCursorShape(qreal angle);


protected:

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;


private:
    /// 绘制矩形框的颜色
    QColor color_;
    QRectF rect_;
    QString name_;
    QString id_;
    QRectF dash_box_;

    QSizeF item_size_;
    CursorType cursor_type_;
    ItemOperation operation_;
    QPointF press_pos_;
    qreal item_ratio_;  //Item的横纵比
    int pad_size_ = 10;  //外框矩形距Item的pad
    int control_point_size_ = 10;  //四角缩放控制点图像直径
    int ratio_line_len_ = 20;   //旋转点距离外框矩形的距离
    int m_RotateEllipseWidth = 20;   //旋转点图像直径
    QPixmap m_RotatePixmap;   //存放旋转点图标
    //存放Item选中时四个缩放控制点和旋转控制点的绘制位置
    QRectF m_TopLeftRect, m_TopRightRect, m_BottomLeftRect, m_BottomRightRect, m_RotateRect;
    qreal m_TopLeftAngle, m_TopRightAngle, m_BottomLeftAngle, m_BottomRightAngle, m_RotateAngle;
    //鼠标悬浮在旋转控制点时的光标样式
    QCursor rotate_hover_cursor_;
    //鼠标点击拖动旋转控制点时的光标样式
    QCursor rotate_press_cursor_;
    //记录Item添至Scene时，Item左上角在Scene的位置
    QPointF current_scene_pos_;
    QTransform transform_;

};




//
// Created by AC on 2024/3/7.
//

#include "arectitem.h"
#include <QUuid>
#include <QVector2D>
#include <utility>

ARectItem::ARectItem(QString tag_id, QString tag_name, const QRectF &rect) :
        rect_(rect),
        color_(Qt::red),
        tag_id_(std::move(tag_id)),
        tag_name_(std::move(tag_name)) {
    this->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    this->setCacheMode(QGraphicsItem::NoCache);
    this->item_ratio_ = rect.width() / rect.height();
    this->setAcceptHoverEvents(true);

    left_top_ = rect.topLeft();

    this->top_right_angle_ = qAtan(this->item_ratio_);

    this->top_left_angle_ = M_PI - this->top_right_angle_;

    this->bottom_right_angle_ = -1.0 * this->top_right_angle_;

    this->bottom_left_angle_ = -1.0 * this->top_left_angle_;

    id_ = tag_id_;

    update_item();
}

void ARectItem::set_tag_id(const QString &name) {
    tag_id_ = name;
}

QString ARectItem::get_tag_id() {
    return tag_id_;
}

QString ARectItem::get_id() {
    return id_;
}

QRectF ARectItem::boundingRect() const {
    QRectF rect = dash_box_;
//    if (this->isSelected()) {
//        rect.adjust(-this->control_point_size_,
//                    -this->control_point_size_,
//                    this->control_point_size_,
//                    this->control_point_size_);
//        rect.adjust(0, 0, 0, this->ratio_line_len_ + this->rotate_ellipse_width_);
//    }
    return rect;
}

void ARectItem::set_color(const QColor &color) {
    color_ = color;
}

void ARectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen;
    pen.setWidth(1);
    QColor brash_color(color_);
    brash_color.setAlpha(30);
    pen.setColor(color_);
    painter->setPen(pen);
    painter->setBrush(brash_color);
    painter->drawRect(rect_);

    if (this->isSelected()) {
        // 边框区域颜色
        painter->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(dash_box_);
        painter->drawLine(QLineF(QPointF(dash_box_.left() + dash_box_.width() / 2,
                                         dash_box_.bottom()),
                                 QPointF(dash_box_.left() + dash_box_.width() / 2,
                                         dash_box_.bottom() + this->ratio_line_len_)));
        painter->setPen(Qt::red);
        painter->setBrush(Qt::red);
        // left-top
        painter->drawEllipse(this->top_left_rect_);
        //left-bottom
        painter->drawEllipse(this->bottom_left_rect_);
        //right-top
        painter->drawEllipse(this->top_right_rect_);
        //right-bottom
        painter->drawEllipse(this->bottom_right_rect_);
        painter->setPen(Qt::yellow);
        painter->setBrush(Qt::yellow);
        // rotate
        painter->drawEllipse(this->rotate_rect_);
    }
}

void ARectItem::update_item() {

    dash_box_ = QRectF(rect_.x() - pad_size_,
                       rect_.y() - pad_size_,
                       rect_.width() + 2 * pad_size_,
                       rect_.height() + 2 * pad_size_);
    this->top_left_rect_ = QRectF(dash_box_.x() - control_point_size_ / 2.0,
                                  dash_box_.y() - control_point_size_ / 2.0,
                                  control_point_size_,
                                  control_point_size_);
    this->bottom_left_rect_ = QRectF(dash_box_.x() - control_point_size_ / 2.0,
                                     dash_box_.bottomLeft().y() - control_point_size_ / 2.0,
                                     control_point_size_,
                                     control_point_size_);
    this->top_right_rect_ = QRectF(dash_box_.topRight().x() - control_point_size_ / 2.0,
                                   dash_box_.y() - control_point_size_ / 2.0,
                                   control_point_size_,
                                   control_point_size_);
    this->bottom_right_rect_ = QRectF(dash_box_.bottomRight().x() - control_point_size_ / 2.0,
                                      dash_box_.bottomRight().y() - control_point_size_ / 2.0,
                                      control_point_size_,
                                      control_point_size_);
    this->rotate_rect_ = QRectF(dash_box_.left() + dash_box_.width() / 2.0 - rotate_ellipse_width_ / 2.0,
                                dash_box_.bottom() + ratio_line_len_,
                                rotate_ellipse_width_,
                                rotate_ellipse_width_);
    update();
}


void ARectItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (is_moveable_ && cursor_type_ == CursorType::HAND_OPEN_CURSOR) {
        press_pos_ = event->scenePos();
        cursor_type_ = CursorType::HAND_CLOSE_CURSOR;
        setCursor(Qt::ClosedHandCursor);
        operation_ = ItemOperation::MOVE;
    } else if ((this->cursor_type_ == RESIZE_LEFT_TOP_CURSOR) | (this->cursor_type_ == RESIZE_RIGHT_TOP_CURSOR) | \
            (this->cursor_type_ == RESIZE_LEFT_BOTTOM_CURSOR) | (this->cursor_type_ == RESIZE_RIGHT_BOTTOM_CURSOR)) {
        this->press_pos_ = event->scenePos();
        this->operation_ = RESIZE;
    }
    QGraphicsItem::mousePressEvent(event);
}

void ARectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    prepareGeometryChange();
    if(!is_moveable_){
        return;
    }
    switch (operation_) {
        case ItemOperation::MOVE: {
            rect_ = QRectF(left_top_ + event->scenePos() - this->press_pos_,
                           QSizeF(rect_.width(), rect_.height()));
            update_item();

            break;
        }
        case ItemOperation::RESIZE: {
            switch (this->cursor_type_) {
                case RESIZE_LEFT_TOP_CURSOR: {
                    resize_and_check_rect(event->scenePos(), QPointF(rect_.bottomRight()), rect_);
                    break;
                }
                case RESIZE_RIGHT_TOP_CURSOR: {
                    resize_and_check_rect(QPointF(rect_.left(), event->scenePos().y()),
                                          QPointF(event->scenePos().x(), rect_.bottom()), rect_);
                    break;
                }
                case RESIZE_LEFT_BOTTOM_CURSOR: {
                    resize_and_check_rect(QPointF(event->scenePos().x(), rect_.top()),
                                          QPointF(rect_.right(), event->scenePos().y()), rect_);
                    break;
                }
                case RESIZE_RIGHT_BOTTOM_CURSOR: {
                    resize_and_check_rect(rect_.topLeft(), event->scenePos(), rect_);
                    break;
                }
                default:
                    break;
            }
            update_item();
            break;
        }
        default:
            break;
    }
    emit item_changed_signal();
    QGraphicsItem::mouseMoveEvent(event);
}

void ARectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    this->clearFocus();
    this->setCursor(Qt::ArrowCursor);
    this->cursor_type_ = CursorType::ARROW_CURSOR;
    this->operation_ = ItemOperation::NONE;
    left_top_ = rect_.topLeft();
    QGraphicsItem::mouseReleaseEvent(event);
}

void ARectItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    if (!this->isSelected()) {
        this->cursor_type_ = HAND_OPEN_CURSOR;
        this->setCursor(Qt::OpenHandCursor);
    } else {
        QPointF pos = event->pos();
        QMarginsF pad = QMarginsF(1, 1, 1, 1);
        if ((this->top_left_rect_ + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_LEFT_TOP_CURSOR;
            this->setCursor(Qt::SizeFDiagCursor);
        } else if ((this->bottom_right_rect_ + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_RIGHT_BOTTOM_CURSOR;
            this->setCursor(Qt::SizeFDiagCursor);
        } else if ((this->top_right_rect_ + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_RIGHT_TOP_CURSOR;
            this->setCursor(Qt::SizeBDiagCursor);
        } else if ((this->bottom_left_rect_ + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_LEFT_BOTTOM_CURSOR;
            this->setCursor(Qt::SizeBDiagCursor);
        } else {
            if (dash_box_.contains(pos)) {
                this->cursor_type_ = HAND_OPEN_CURSOR;
                this->setCursor(Qt::OpenHandCursor);
                this->setToolTip(QString("移动"));
            }
        }
    }
    // 鼠标进入item后隐藏cross
    emit mouse_hover_signal(true);
    QGraphicsItem::hoverEnterEvent(event);
}

void ARectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    if (!this->isSelected()) {
        this->cursor_type_ = HAND_OPEN_CURSOR;
        this->setCursor(Qt::OpenHandCursor);
    } else {
        QPointF pos = event->pos();
        QMarginsF pad = QMarginsF(1, 1, 1, 1);
        if ((this->top_left_rect_ + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_LEFT_TOP_CURSOR;
            this->setCursor(Qt::SizeFDiagCursor);
        } else if ((this->bottom_right_rect_ + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_RIGHT_BOTTOM_CURSOR;
            this->setCursor(Qt::SizeFDiagCursor);
        } else if ((this->top_right_rect_ + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_RIGHT_TOP_CURSOR;
            this->setCursor(Qt::SizeBDiagCursor);
        } else if ((this->bottom_left_rect_ + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_LEFT_BOTTOM_CURSOR;
            this->setCursor(Qt::SizeBDiagCursor);
        } else {
            if (dash_box_.contains(pos)) {
                this->cursor_type_ = HAND_OPEN_CURSOR;
                this->setCursor(Qt::OpenHandCursor);
            }
        }
    }
    QGraphicsItem::hoverMoveEvent(event);
}

void ARectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    this->cursor_type_ = ARROW_CURSOR;
    this->setCursor(Qt::ArrowCursor);
    // 鼠标离开后取消cross隐藏状态
    emit mouse_hover_signal(false);
    QGraphicsItem::hoverLeaveEvent(event);
}

QVariant ARectItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    prepareGeometryChange();
    return QGraphicsItem::itemChange(change, value);
}

void ARectItem::resize_and_check_rect(const QPointF &p1, const QPointF &p2, QRectF &rect) {
    QRectF temp_rect(p1, p2);
    if (temp_rect.width() < 5 || temp_rect.height() < 5) return;
    rect = temp_rect;
}

QRectF ARectItem::get_inner_rect() {
    return rect_;
}

void ARectItem::move_by(const QPointF &point) {
    rect_ = QRectF(left_top_ + point,
                   QSizeF(rect_.width(), rect_.height()));
    left_top_ = rect_.topLeft();
    update_item();
}

void ARectItem::set_tag_name(const QString &tag_name) {

    tag_name_ = tag_name;
}

QString ARectItem::get_tag_name() {
    return tag_name_;
}

void ARectItem::set_moveable(bool is_moveable){
    is_moveable_ = is_moveable;
}




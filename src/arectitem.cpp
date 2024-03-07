//
// Created by AC on 2024/3/7.
//

#include "arectitem.h"
#include <QUuid>
#include <QVector2D>

ARectItem::ARectItem(const QRectF &rect) :
        rect_(rect),
        item_size_(rect.size()),
        color_(Qt::red),
        id_(QUuid::createUuid().toString()) {
    this->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);

    this->setCacheMode(QGraphicsItem::NoCache);
    this->item_ratio_ = rect.width() / rect.height();
    this->setAcceptHoverEvents(true);
    this->m_RotatePixmap = QPixmap(":/images/rotate.png");
    this->rotate_hover_cursor_ = QCursor(
            QPixmap(":/images/rotate_hover.png").scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    this->rotate_hover_cursor_ = QCursor(
            QPixmap(":/images/rotate_press.png").scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    dash_box_ = QRectF(rect.x() - pad_size_,
                       rect.y() - pad_size_,
                       rect.width() + 2 * pad_size_,
                       rect.height() + 2 * pad_size_);

    // 坐标很奇妙
    current_scene_pos_ = this->pos();

    this->m_TopRightAngle = qAtan(this->item_ratio_);

    this->m_TopLeftAngle = M_PI - this->m_TopRightAngle;

    this->m_BottomRightAngle = -1.0 * this->m_TopRightAngle;

    this->m_BottomLeftAngle = -1.0 * this->m_TopLeftAngle;

    item_resize();
}

void ARectItem::set_name(const QString &name) {
    name_ = name;
}

QString ARectItem::get_name() {
    return name_;
}

QString ARectItem::get_id() {
    return id_;
}

QRectF ARectItem::boundingRect() const {

    QRectF rect = dash_box_;
    if (this->isSelected()) {
        rect.adjust(-this->control_point_size_, -this->control_point_size_, this->control_point_size_,
                    this->control_point_size_);
        rect.adjust(0, 0, 0, this->ratio_line_len_ + this->m_RotateEllipseWidth);
    }
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
        painter->setPen(QPen(Qt::blue, 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);

        dash_box_ = QRectF(rect_.x() - pad_size_,
                           rect_.y() - pad_size_,
                           rect_.width() + 2 * pad_size_,
                           rect_.height() + 2 * pad_size_);
        painter->drawRect(dash_box_);
        painter->drawRect(boundingRect());
        painter->drawLine(QLineF(QPointF(dash_box_.left() + dash_box_.width() / 2,
                                         dash_box_.bottom()),
                                 QPointF(dash_box_.left() + dash_box_.width() / 2,
                                         dash_box_.bottom() + this->ratio_line_len_)));
        painter->setPen(Qt::red);
        painter->setBrush(Qt::red);

        // left-top
        painter->drawEllipse(this->m_TopLeftRect);
        //left-bottom
        painter->drawEllipse(this->m_BottomLeftRect);
        //right-top
        painter->drawEllipse(this->m_TopRightRect);
        //right-bottom
        painter->drawEllipse(this->m_BottomRightRect);

        painter->drawPixmap(this->m_RotateRect, this->m_RotatePixmap, this->m_RotatePixmap.rect());
    }
}

void ARectItem::item_resize() {

    this->m_TopLeftRect = QRectF(dash_box_.x() - control_point_size_ / 2.0,
                                 dash_box_.y() - control_point_size_ / 2.0,
                                 control_point_size_,
                                 control_point_size_);
    this->m_BottomLeftRect = QRectF(dash_box_.x() - control_point_size_ / 2.0,
                                    dash_box_.bottomLeft().y() - control_point_size_ / 2.0,
                                    control_point_size_,
                                    control_point_size_);
    this->m_TopRightRect = QRectF(dash_box_.topRight().x() - control_point_size_ / 2.0,
                                  dash_box_.y() - control_point_size_ / 2.0,
                                  control_point_size_,
                                  control_point_size_);
    this->m_BottomRightRect = QRectF(dash_box_.bottomRight().x() - control_point_size_ / 2.0,
                                     dash_box_.bottomRight().y() - control_point_size_ / 2.0,
                                     control_point_size_,
                                     control_point_size_);
    this->m_RotateRect = QRectF(dash_box_.left() + dash_box_.width() / 2.0 - m_RotateEllipseWidth / 2.0,
                                dash_box_.bottom() + ratio_line_len_,
                                m_RotateEllipseWidth,
                                m_RotateEllipseWidth);


}

Qt::CursorShape ARectItem::GetResizeCursorShape(qreal angle) {
    qreal sector = M_PI_4;
    qreal value = angle + sector / 2;
    qreal theta = fmod(value, M_PI);
    if (theta < 0) {
        theta += M_PI;
    }
    int index = static_cast<int>(floor(theta / sector));
//    qDebug()<<qRadiansToDegrees(angle)<<qRadiansToDegrees(theta)<<"index"<<index;
    switch (index) {
        case 0:
            return Qt::SizeHorCursor;
        case 1:
            return Qt::SizeBDiagCursor;
        case 2:
            return Qt::SizeVerCursor;
        case 3:
            return Qt::SizeFDiagCursor;
        default:
            return Qt::ArrowCursor;
    }
}

void ARectItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    this->setFocus();
    if (cursor_type_ == CursorType::HAND_OPEN_CURSOR) {
        press_pos_ = event->scenePos();
        cursor_type_ = CursorType::HAND_CLOSE_CURSOR;
        setCursor(Qt::ClosedHandCursor);
        operation_ = ItemOperation::MOVE;
    } else if ((this->cursor_type_ == RESIZE_LEFT_TOP_CURSOR) | (this->cursor_type_ == RESIZE_RIGHT_TOP_CURSOR) | \
            (this->cursor_type_ == RESIZE_LEFT_BOTTOM_CURSOR) | (this->cursor_type_ == RESIZE_RIGHT_BOTTOM_CURSOR)) {
        this->press_pos_ = event->scenePos();
        this->operation_ = RESIZE;
    } else if (this->cursor_type_ == ROTATE_CURSOR) {
        this->press_pos_ = event->pos();
        this->transform_ = this->transform();
//        qDebug()<<"angle"<<qRadiansToDegrees(qAsin(this->m_Transform.m12()));
        this->operation_ = ROTATE;
        this->setCursor(this->rotate_press_cursor_);
    }
    QGraphicsItem::mousePressEvent(event);
}

void ARectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    switch (operation_) {
        case ItemOperation::MOVE: {
            qDebug() << "===========================================";
            qDebug() << "current_scene_pos_" << current_scene_pos_;
            qDebug() << "leftTop:" << rect_.topLeft();
            qDebug() << "Pose" << event->pos();
            qDebug() << "scenePose" << event->scenePos();
            qDebug() << "mapToScene:" << this->mapToScene(event->pos());
            qDebug() << "mapToScene1:" << this->mapToScene(event->scenePos());
            this->setPos(this->current_scene_pos_ + event->scenePos() - this->press_pos_);
            this->update();
            break;
        }
        case ItemOperation::RESIZE: {
            QPointF diff = event->pos() - this->press_pos_;
            if (abs(diff.x()) < 5 || abs(diff.y()) < 5) return;
//            if (abs(diff.x()) > item_size_.width() - 5 || abs(diff.y()) > item_size_.height() - 5) return;
            switch (this->cursor_type_) {
                case RESIZE_LEFT_TOP_CURSOR: {
                    rect_ = QRectF(event->scenePos(), QPointF(rect_.bottomRight()));
                    break;
                }
                case RESIZE_RIGHT_TOP_CURSOR: {
                    rect_ = QRectF(QPointF(rect_.left(), event->scenePos().y()),
                                   QPointF(event->scenePos().x(), rect_.bottom()));
                    break;
                }
                case RESIZE_LEFT_BOTTOM_CURSOR: {
                    rect_ = QRectF(QPointF(event->scenePos().x(), rect_.top()),
                                   QPointF(rect_.right(), event->scenePos().y()));
                    break;
                }
                case RESIZE_RIGHT_BOTTOM_CURSOR: {
                    rect_ = QRectF(rect_.topLeft(), event->scenePos());
                    break;
                }
                default:
                    break;
            }
            item_resize();
            this->update();
            break;
        }
        case ItemOperation::ROTATE: {
            auto startVect = QVector2D(this->press_pos_);
            startVect.normalize();
            auto endVect = QVector2D(event->pos());
            endVect.normalize();
            qreal value = QVector2D::dotProduct(startVect, endVect);
            if (value > 1.0)
                value = 1.0;
            else if (value < -1.0)
                value = -1.0;
            qreal angle = qRadiansToDegrees(qAcos(value));
            QVector3D vect = QVector3D::crossProduct(QVector3D(startVect, 1.0), QVector3D(endVect, 1.0));
            if (vect.z() < 0)
                angle *= -1.0;
            this->transform_.rotate(angle);
            this->setTransform(this->transform_);
            this->update();
            break;
        }
        default:
            break;
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void ARectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    this->clearFocus();
    this->setCursor(Qt::ArrowCursor);
    this->cursor_type_ = CursorType::ARROW_CURSOR;
    this->operation_ = ItemOperation::NONE;
    this->current_scene_pos_ = this->pos();
    this->item_size_ = rect_.size();
    QGraphicsItem::mouseReleaseEvent(event);
}

void ARectItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    if (!this->isSelected()) {
        this->cursor_type_ = HAND_OPEN_CURSOR;
        this->setCursor(Qt::OpenHandCursor);
    } else {
        QPointF pos = event->pos();
        QRectF rect = rect_;
        qreal angle = qAtan2(this->transform().m12(), this->transform().m11());
        QMarginsF pad = QMarginsF(1, 1, 1, 1);
        rect.adjust(-this->pad_size_, -this->pad_size_, this->pad_size_, this->pad_size_);
        if ((this->m_TopLeftRect + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_LEFT_TOP_CURSOR;
            angle = this->m_TopLeftAngle - angle;
            this->setCursor(GetResizeCursorShape(angle));
            this->setToolTip(QString("缩放"));
        } else if ((this->m_BottomRightRect + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_RIGHT_BOTTOM_CURSOR;
            angle = this->m_BottomRightAngle - angle;
            this->setCursor(GetResizeCursorShape(angle));
            this->setToolTip(QString("缩放"));
        } else if ((this->m_TopRightRect + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_RIGHT_TOP_CURSOR;
            angle = this->m_TopRightAngle - angle;
            this->setCursor(GetResizeCursorShape(angle));
            this->setToolTip(QString("缩放"));
        } else if ((this->m_BottomLeftRect + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_LEFT_BOTTOM_CURSOR;
            angle = this->m_BottomLeftAngle - angle;
            this->setCursor(GetResizeCursorShape(angle));
            this->setToolTip(QString("缩放"));
        } else if ((this->m_RotateRect + pad).contains(pos)) {
            this->cursor_type_ = ROTATE_CURSOR;
            this->setCursor(this->rotate_hover_cursor_);
            this->setToolTip(QString("旋转"));
        } else {
            if (rect.contains(pos)) {
                this->cursor_type_ = HAND_OPEN_CURSOR;
                this->setCursor(Qt::OpenHandCursor);
                this->setToolTip(QString("移动"));
            }
        }
    }
    QGraphicsItem::hoverEnterEvent(event);
}

void ARectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    if (!this->isSelected()) {
        this->cursor_type_ = HAND_OPEN_CURSOR;
        this->setCursor(Qt::OpenHandCursor);
    } else {
        QPointF pos = event->pos();
        QRectF rect = rect_;
        qreal angle = qAtan2(this->transform().m12(), this->transform().m11());
        QMarginsF pad = QMarginsF(1, 1, 1, 1);
        rect.adjust(-this->pad_size_, -this->pad_size_, this->pad_size_, this->pad_size_);
        if ((this->m_TopLeftRect + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_LEFT_TOP_CURSOR;
            angle = this->m_TopLeftAngle - angle;
            this->setCursor(GetResizeCursorShape(angle));
        } else if ((this->m_BottomRightRect + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_RIGHT_BOTTOM_CURSOR;
            angle = this->m_BottomRightAngle - angle;
            this->setCursor(GetResizeCursorShape(angle));
        } else if ((this->m_TopRightRect + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_RIGHT_TOP_CURSOR;
            angle = this->m_TopRightAngle - angle;
            this->setCursor(GetResizeCursorShape(angle));
        } else if ((this->m_BottomLeftRect + pad).contains(pos)) {
            this->cursor_type_ = RESIZE_LEFT_BOTTOM_CURSOR;
            angle = this->m_BottomLeftAngle - angle;
            this->setCursor(GetResizeCursorShape(angle));
        } else if ((this->m_RotateRect + pad).contains(pos)) {
            this->cursor_type_ = ROTATE_CURSOR;
            this->setCursor(this->rotate_hover_cursor_);
        } else {
            if (rect.contains(pos)) {
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
    QGraphicsItem::hoverLeaveEvent(event);
}

QVariant ARectItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {

    if (change == QGraphicsItem::ItemSelectedChange)
        prepareGeometryChange();
    return QGraphicsItem::itemChange(change, value);
}

void ARectItem::keyPressEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key_R) {
        qDebug() << "asdasd";
        moveBy(1, 1);
    }

    QGraphicsItem::keyPressEvent(event);
}



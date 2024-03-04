#include "agraphicsview.h"
#include "ui_agraphicsview.h"
#include <QPixmap>
#include <QMessageBox>
#include <QUuid>

AGraphicsView::AGraphicsView(QWidget *parent) :
        QGraphicsView(parent),
        ui(new Ui::AGraphicsView) {
    ui->setupUi(this);
    this->setScene(new QGraphicsScene(this));
    this->setMouseTracking(true);
    temp_canvas_ = new TempGraphicsItem(QSize(3000, 3000));
    temp_canvas_->setZValue(1);
    this->scene()->addItem(temp_canvas_);
    cross_item_ = new CrossItem();
    // 保证十字标线在最上方
    cross_item_->setZValue(1);
    this->scene()->addItem(cross_item_);

    this->setBackgroundBrush(QColor(53, 53, 53));
    this->setRenderHint(QPainter::Antialiasing);
    this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    this->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
//    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    this->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
}

void AGraphicsView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        last_point_ = event->pos();
        qDebug() << "pressed: " << last_point_;
    }
    QGraphicsView::mousePressEvent(event);
}

void AGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    current_point_ = event->pos();
    if (draw_rect_checked_) {
        if (event->buttons() & Qt::LeftButton) {
//            temp_canvas_->draw_shape(mapToScene(last_point_), mapToScene(current_point_));
        }
        cross_item_->draw_shape(mapToScene(event->pos()).toPoint(), this->size().width(), this->size().height());
    }
    emit send_position_signal(current_point_, mapToScene(current_point_).toPoint());
    QGraphicsView::mouseMoveEvent(event);
}

void AGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    if (draw_rect_checked_ && event->button() == Qt::LeftButton) {
        temp_canvas_->clear();
        auto *item1 = new QGraphicsRectItem();
        // 0 代表 id
        item1->setData(0, QUuid::createUuid().toString());
        // 1 代表 name
        item1->setData(1, "");
        item1->setPen(QPen(Qt::red));
        item1->setBrush(QBrush(QColor(255, 0, 0, 30)));
        item1->setRect(QRectF(mapToScene(last_point_), mapToScene(event->pos())));
        this->scene()->addItem(item1);
        emit send_draw_final_signal(item1);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void AGraphicsView::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() == 0) {
            event->ignore();
            return;
        }
        if (event->angleDelta().y() > 0.0) {
            scale_up();
        } else {
            scale_down();
        }
    }
}

void AGraphicsView::paintEvent(QPaintEvent *event) {
    QGraphicsView::paintEvent(event);
}


void AGraphicsView::add_image_item(const QPixmap &pix) {
//    this->scene()->clear();
//    this->scene()->clearSelection();
    index_id = 0;
    auto *background_img_item = new QGraphicsPixmapItem(pix);
    this->scene()->setSceneRect(QRectF(QPointF(0, 0), pix.size()));
    background_img_item->setPos(0, 0);
    this->scene()->addItem(background_img_item);
}

void AGraphicsView::set_draw_shape_status(bool checked_status) {
    draw_rect_checked_ = checked_status;
    if (!draw_rect_checked_ and cross_item_) {
        scene()->removeItem(cross_item_);
    }
    if (draw_rect_checked_ and cross_item_) {
        scene()->addItem(cross_item_);
    }
}

AGraphicsView::~AGraphicsView() {
    delete ui;
}

void AGraphicsView::setup_scale(double scale) {
    scale = std::max(scale_range_.minimum, std::min(scale_range_.maximum, scale));
    if (scale <= 0)
        return;
    if (scale == transform().m11())
        return;
    QTransform matrix;
    matrix.scale(scale, scale);
    setTransform(matrix, false);
}

void AGraphicsView::scale_down() {
    double const step = 1.2;
    double const factor = std::pow(step, -1.0);

    if (scale_range_.minimum > 0) {
        QTransform t = transform();
        t.scale(factor, factor);
        if (t.m11() <= scale_range_.minimum) {
            setup_scale(t.m11());
            return;
        }
    }
    scale(factor, factor);
}

void AGraphicsView::scale_up() {
    double const step = 1.2;
    double const factor = std::pow(step, 1.0);
    if (scale_range_.maximum > 0) {
        QTransform t = transform();
        t.scale(factor, factor);
        if (t.m11() >= scale_range_.maximum) {
            setup_scale(t.m11());
            return;
        }
    }
    scale(factor, factor);
}

void AGraphicsView::set_scale_range(double minimum, double maximum) {
    if (maximum < minimum)
        std::swap(minimum, maximum);
    minimum = std::max(0.0, minimum);
    maximum = std::max(0.0, maximum);

    scale_range_ = {minimum, maximum};
    setup_scale(transform().m11());
}

void AGraphicsView::drawBackground(QPainter *painter, const QRectF &r) {

    QGraphicsView::drawBackground(painter, r);
    auto draw_grid = [&](double grid_step) {
        QRect windowRect = rect();
        QPointF tl = mapToScene(windowRect.topLeft());
        QPointF br = mapToScene(windowRect.bottomRight());

        double left = std::floor(tl.x() / grid_step - 0.5);
        double right = std::floor(br.x() / grid_step + 1.0);
        double bottom = std::floor(tl.y() / grid_step - 0.5);
        double top = std::floor(br.y() / grid_step + 1.0);

        // vertical lines
        for (int xi = int(left); xi <= int(right); ++xi) {
            QLineF line(xi * grid_step, bottom * grid_step, xi * grid_step, top * grid_step);
            painter->drawLine(line);
        }

        // horizontal lines
        for (int yi = int(bottom); yi <= int(top); ++yi) {
            QLineF line(left * grid_step, yi * grid_step, right * grid_step, yi * grid_step);
            painter->drawLine(line);
        }
    };

    QPen pen1(QColor(60, 60, 60), 1.0);
    painter->setPen(pen1);
    draw_grid(15);

    QPen pen(QColor(0, 0, 0), 1.0);
    painter->setPen(pen);
    draw_grid(150);
}

void AGraphicsView::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Shift:
            this->setDragMode(QGraphicsView::ScrollHandDrag);
            break;
        default:
            break;
    }
    QGraphicsView::keyPressEvent(event);
}

void AGraphicsView::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Shift:
            this->setDragMode(QGraphicsView::RubberBandDrag);
            break;
        default:
            break;
    }
    QGraphicsView::keyReleaseEvent(event);
}

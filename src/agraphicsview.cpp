#include "agraphicsview.h"
#include "ui_agraphicsview.h"
#include <QPixmap>
#include <QMessageBox>
#include <QUuid>
#include "config.h"


AGraphicsView::AGraphicsView(QWidget *parent) :
        QGraphicsView(parent),
        ui(new Ui::AGraphicsView),
        scale_range_({Config::scale_range_min, Config::scale_range_max}),
        default_scene_size_(Config::default_scene_size) {
    ui->setupUi(this);
    this->setScene(new QGraphicsScene());
    setSceneRect(0, 0, Config::default_scene_size, Config::default_scene_size);
    this->setMouseTracking(true);
    temp_canvas_ = new TempGraphicsItem(QSize(default_scene_size_, default_scene_size_));
    this->scene()->addItem(temp_canvas_);

    // 保证十字标线在最上方
    cross_item_ = new CrossItem();
    cross_item_->setZValue(1);
    this->scene()->addItem(cross_item_);

    this->setCacheMode(QGraphicsView::CacheBackground);
    this->setBackgroundBrush(Config::view_bg_color);
    this->setRenderHint(QPainter::Antialiasing);
    this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    this->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
//    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    this->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
            temp_canvas_->draw_shape(this->mapToScene(last_point_), this->mapToScene(current_point_));
        }
        cross_item_->draw_shape(this->mapToScene(current_point_).toPoint(),
                                this->mapToScene(QPoint(0, 0)).toPoint(),
                                this->mapToScene(QPoint(this->width(), this->height())).toPoint());
    }
    emit send_position_signal(current_point_, this->mapToScene(current_point_).toPoint());
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
        item1->setBrush(box_color);
        item1->setRect(QRectF(this->mapToScene(last_point_), this->mapToScene(event->pos())));
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
    this->scene()->clear();
    this->scene()->clearSelection();
    auto *background_img_item = new QGraphicsPixmapItem(pix);
    this->setSceneRect(pix.rect());
    background_img_item->setPos(0, 0);
    this->scene()->addItem(background_img_item);
    centerOn(this->sceneRect().center());
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
    double const factor = std::pow(Config::scale_step, -1.0);

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
    double const factor = std::pow(Config::scale_step, 1.0);
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
        QPointF tl = this->mapToScene(windowRect.topLeft());
        QPointF br = this->mapToScene(windowRect.bottomRight());

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

    QPen pen1(Config::view_inter_line_color, 1.0);
    painter->setPen(pen1);
    draw_grid(Config::inter_grid_step);

    QPen pen(Config::view_border_line_color, 1.0);
    painter->setPen(pen);
    draw_grid(Config::border_grid_step);
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

void AGraphicsView::center_scene() {
    if (this->scene()) {
//        this->scene()->setSceneRect(QRectF());
//        QRectF sceneRect = this->scene()->sceneRect();
//        if (sceneRect.width() > this->rect().width() || sceneRect.height() > this->rect().height()) {
//            fitInView(sceneRect, Qt::KeepAspectRatio);
//        }
        centerOn(this->scene()->sceneRect().center());
    }
}

void AGraphicsView::showEvent(QShowEvent *event) {
    center_scene();
    QGraphicsView::showEvent(event);
}

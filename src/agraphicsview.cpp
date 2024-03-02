#include "agraphicsview.h"
#include "ui_agraphicsview.h"
#include <QPixmap>
#include <QMessageBox>
#include <QUuid>

AGraphicsView::AGraphicsView(QWidget *parent) :
        QGraphicsView(parent),
        ui(new Ui::AGraphicsView) {
    ui->setupUi(this);
    setScene(new QGraphicsScene(this));
    setMouseTracking(true);
    temp_canvas_ = new TempGraphicsItem(QSize(3000, 3000));
    temp_canvas_->setZValue(1);
    this->scene()->addItem(temp_canvas_);
    cross_item_ = new CrossItem();
    // 保证十字标线在最上方
    cross_item_->setZValue(1);
    this->scene()->addItem(cross_item_);
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
            temp_canvas_->draw_shape(mapToScene(last_point_), mapToScene(current_point_));
        }
    }
    cross_item_->draw_shape(mapToScene(event->pos()).toPoint(), this->size().width(), this->size().height());
    emit send_position_signal(current_point_, mapToScene(current_point_).toPoint());
    QGraphicsView::mouseMoveEvent(event);
}

void AGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    if (draw_rect_checked_) {
        temp_canvas_->clear();
        auto *item1 = new QGraphicsRectItem();
        // 0 代表 id
        item1->setData(0, QUuid::createUuid().toString());
        // 1 代表 name
        item1->setData(1, "");
        item1->setPen(QPen(Qt::red));
        item1->setRect(QRectF(mapToScene(last_point_), mapToScene(event->pos())));
        this->scene()->addItem(item1);
        emit send_draw_final_signal(item1);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void AGraphicsView::wheelEvent(QWheelEvent *e) {
//    scale_factor = this->matrix().m11();
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->angleDelta().y() > 0) {
            this->scale(1.2, 1.2);
        } else
            this->scale(0.8, 0.8);
        e->accept();
    } else {
        QGraphicsView::wheelEvent(e);
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
}

AGraphicsView::~AGraphicsView() {
    delete ui;
}

#include "agraphicsview.h"
#include "ui_agraphicsview.h"
#include <QPixmap>
#include <QMessageBox>

AGraphicsView::AGraphicsView(QWidget *parent) :
        QGraphicsView(parent),
        ui(new Ui::AGraphicsView) {
    ui->setupUi(this);
    setScene(new QGraphicsScene(this));
    setMouseTracking(true);
    temp_canvas_ = new TempGraphicsItem(this->size());
    this->scene()->addItem(temp_canvas_);
    cross_item_ = new CrossItem();
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
    // 必须手动刷新，不然不会有十字准星
    this->scene()->update();
    QGraphicsView::mouseMoveEvent(event);
}

void AGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    if (draw_rect_checked_) {
        temp_canvas_->clear();
        auto *item1 = new QGraphicsRectItem();
        item1->setRect(QRectF(mapToScene(last_point_), mapToScene(event->pos())));
        this->scene()->addItem(item1);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void AGraphicsView::wheelEvent(QWheelEvent *e) {
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->angleDelta().y() > 0)
            this->scale(1.2, 1.2);
        else
            this->scale(0.8, 0.8);
        e->accept();
    } else {
        QGraphicsView::wheelEvent(e);
    }
}

void AGraphicsView::paintEvent(QPaintEvent *event) {
//    QPainter painter(this->viewport());
//    QPen pen;
//    pen.setWidth(1);
//    pen.setColor(Qt::red);
//    painter.setPen(pen);
//    //绘制横向线
//    painter.drawLine(0, current_point_.y(), width(), current_point_.y());
//    //绘制纵向线
//    painter.drawLine(current_point_.x(), 0, current_point_.x(), height());
    QGraphicsView::paintEvent(event);
}

void AGraphicsView::add_image_item(const QPixmap &pix) {
//    this->scene()->clear();
//    this->scene()->clearSelection();
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

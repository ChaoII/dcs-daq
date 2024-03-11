//
// Created by AC on 2024/3/11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ARectListItem.h" resolved

#include "arectlistitem.h"
#include "ui_ARectListItem.h"


ARectListItem::ARectListItem(QWidget *parent) :
        QWidget(parent), ui(new Ui::ARectListItem) {
    ui->setupUi(this);
    this->setAttribute(Qt::WA_StyledBackground, true);
}

ARectListItem::~ARectListItem() {
    delete ui;
}

void ARectListItem::mousePressEvent(QMouseEvent *event) {

    if (event->button() == Qt::LeftButton) {
        if (!(QApplication::keyboardModifiers() & Qt::ControlModifier)) {
            emit clicked_signal();
        }
    }
    set_selected(!selected_status_);
    // 注意，拦截事件
    // QWidget::mousePressEvent(event);
}

void ARectListItem::set_selected(bool status) {
    selected_status_ = status;
    if (selected_status_) {
        setStyleSheet("background-color: rgba(100, 100, 100, 100)");
    } else {
        setStyleSheet("background-color: rgba(160, 160, 160, 40)");
    }
    emit selected_status_change_signal();
}

void ARectListItem::set_item(const QString &name, const QRectF &rect) {
    ui->lb_itemName->setText(name);
    ui->lb_x->setText(QString::number(rect.x()));
    ui->lb_y->setText(QString::number(rect.y()));
    ui->lb_width->setText(QString::number(rect.width()));
    ui->lb_height->setText(QString::number(rect.height()));
}


QLabel *ARectListItem::get_order_label() {
    return ui->lb_order;
}

bool ARectListItem::set_selected_status() {
    return selected_status_;
}

void ARectListItem::update_rect(const QRectF &rect) {
    ui->lb_x->setText(QString::number(rect.x()));
    ui->lb_y->setText(QString::number(rect.y()));
    ui->lb_width->setText(QString::number(rect.width()));
    ui->lb_height->setText(QString::number(rect.height()));
}

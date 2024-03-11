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
        if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
            // Ctrl + 鼠标左键被按下
            // 在这里处理你的逻辑
        } else {
            // 其他情况，可以调用基类的mousePressEvent
            emit clicked_signal();
        }
    }
    set_selected(!selected_status_);
    QWidget::mousePressEvent(event);
}

void ARectListItem::set_selected(bool status) {
    selected_status_ = status;
    if (selected_status_) {
        setStyleSheet("background-color: rgba(100, 100, 100, 100)");
    } else {
        setStyleSheet("background-color: rgba(160, 160, 160, 40)");
    }
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

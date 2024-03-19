//
// Created by AC on 2024/3/11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ARectListItem.h" resolved

#include "arectlistitem.h"
#include "ui_ARectListItem.h"
#include <QLabel>
#include <QFontMetrics>


ARectListItem::ARectListItem(QWidget *parent) :
        QWidget(parent), ui(new Ui::ARectListItem) {
    ui->setupUi(this);
    this->setAttribute(Qt::WA_StyledBackground, true);
    ui->lb_tag_id->setMaximumWidth(250);
    ui->lb_tag_name->setMaximumWidth(250);
    ui->lb_tag_id->setMinimumWidth(150);
    ui->lb_tag_name->setMinimumWidth(150);
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
//     QWidget::mousePressEvent(event);
}

void ARectListItem::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit double_clicked_signal();
    }
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

void ARectListItem::set_item(const QString &tag_id, const QString &tag_name) {
    QFontMetrics fontWidth(ui->lb_tag_id->font());
    auto elide_tag_id = fontWidth.elidedText(tag_id, Qt::ElideRight, ui->lb_tag_id->maximumWidth());
    ui->lb_tag_id->setText(elide_tag_id);
    ui->lb_tag_id->setToolTip(tag_id);

    QFontMetrics fontWidth_(ui->lb_tag_name->font());
    auto elide_tag_name = fontWidth_.elidedText(tag_name, Qt::ElideRight, ui->lb_tag_name->maximumWidth());
    ui->lb_tag_name->setText(elide_tag_name);
    ui->lb_tag_name->setToolTip(tag_name);
}


QLabel *ARectListItem::get_order_label() {
    return ui->lb_order;
}

bool ARectListItem::get_selected_status() {
    return selected_status_;
}


void ARectListItem::set_tag_id(const QString &tag_id) {
    ui->lb_tag_id->setText(tag_id);
}

QString ARectListItem::get_tag_id() {
    return ui->lb_tag_id->text();
}

void ARectListItem::set_tag_name(const QString &tag_name) {

    ui->lb_tag_name->setText(tag_name);

}

QString ARectListItem::get_tag_name() {
    return ui->lb_tag_name->text();
}



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
    setStyleSheet("#lb_itemName{"
                      "border-width:0px 0px 1px 0px;\n"
                      "border-style:solid;\n"
                      "border-color: rgb(200,200,200)"
                  "}\n"
                  "#lb_order, #lb_x, #lb_y, #lb_width{"
                      "border-width:0px 1px 0px 0px;\n"
                      "border-style:solid;\n"
                      "border-color: rgb(200,200,200)"
                  "}\n"
                  "#ARectListItem{"
                      "border:1px solid rgb(255,255,255);\n"
                      "background-color:rgba(160,160,160,40)"
                  "}\n");
}

ARectListItem::~ARectListItem() {
    delete ui;
}

void ARectListItem::mousePressEvent(QMouseEvent *event) {
    emit clicked_signal();
}

void ARectListItem::set_item(const QString &name, const QRectF &rect) {
    ui->lb_itemName->setText(name);
    ui->lb_x->setText(QString::number(rect.x()));
    ui->lb_y->setText(QString::number(rect.y()));
    ui->lb_width->setText(QString::number(rect.width()));
    ui->lb_height->setText(QString::number(rect.height()));
}

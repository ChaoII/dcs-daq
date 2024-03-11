#include "arectlist.h"
#include "ui_ARectList.h"

ARectList::ARectList(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ARectList) {
    ui->setupUi(this);
    setMinimumWidth(200);
    setMaximumWidth(400);
}

ARectList::~ARectList() {

}

ARectListItem *ARectList::add_item(const QString &name, const QRectF &rect) {
    auto rect_list_item = new ARectListItem();
    rect_list_item->set_item(name, rect);
    connect(rect_list_item, &ARectListItem::clicked_signal, this, &ARectList::on_receive_item_selected);
    connect(rect_list_item, &ARectListItem::selected_status_change_signal, this, &ARectList::on_item_row_changed);
    ui->scrollAreaInterLayout->addWidget(rect_list_item);
    return rect_list_item;
}

void ARectList::clear() {
    QLayoutItem *item;
    while ((item = this->layout()->takeAt(0)) != nullptr) {
        delete item->widget();  // 删除控件
        delete item;            // 删除布局项
    }
}

void ARectList::re_set_order() {
    for (int i = 0; i < ui->scrollAreaInterLayout->count(); i++) {
        QLayoutItem *it = ui->scrollAreaInterLayout->itemAt(i);
        auto rect_list_item = dynamic_cast<ARectListItem *>(it->widget());
        if (rect_list_item) {
            rect_list_item->get_order_label()->setText(QString::number(i));
        }
    }
}

void ARectList::clear_item_selected() {
    for (int i = 0; i < ui->scrollAreaInterLayout->count(); i++) {
        QLayoutItem *it = ui->scrollAreaInterLayout->itemAt(i);
        auto rect_list_item = dynamic_cast<ARectListItem *>(it->widget());
        if (rect_list_item) {
            rect_list_item->set_selected(false);
        }
    }
}

void ARectList::mousePressEvent(QMouseEvent *event) {
    clear_item_selected();
    QWidget::mousePressEvent(event);
}

void ARectList::on_receive_item_selected() {
    clear_item_selected();
}

void ARectList::on_item_row_changed() {
    auto rect_list_item = dynamic_cast<ARectListItem *>(sender());
    if (rect_list_item) {
        emit item_change_item(rect_list_item);
    }
}





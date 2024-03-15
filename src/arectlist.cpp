#include "arectlist.h"
#include "ui_ARectList.h"

ARectList::ARectList(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ARectList) {
    ui->setupUi(this);
    setMinimumWidth(180);
    setMaximumWidth(500);
}

ARectList::~ARectList() {

}

ARectListItem *ARectList::add_item(const QString &tag_id, const QString &tag_name) {
    auto rect_list_item = new ARectListItem();
    rect_list_item->set_item(tag_id, tag_name);
    connect(rect_list_item, &ARectListItem::clicked_signal, this, &ARectList::on_receive_item_selected);
    connect(rect_list_item, &ARectListItem::double_clicked_signal, this, &ARectList::on_double_clicked);
    connect(rect_list_item, &ARectListItem::selected_status_change_signal, this, &ARectList::on_item_row_changed);
    ui->scrollAreaInterLayout->addWidget(rect_list_item);
    return rect_list_item;
}

void ARectList::clear() {
    QLayoutItem *item;
    while ((item = ui->scrollAreaInterLayout->layout()->takeAt(0)) != nullptr) {
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
    ui->lb_x->setText(QString::number(0));
    ui->lb_y->setText(QString::number(0));
    ui->lb_w->setText(QString::number(0));
    ui->lb_h->setText(QString::number(0));
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

void ARectList::on_double_clicked() {
    auto item = dynamic_cast<ARectListItem *>(sender());
    if (item) {
        emit item_double_clicked_signal(item);
    }
}

void ARectList::update_rect_coordinate(const QRectF &rect) {
    ui->lb_x->setText(QString::number(rect.x()));
    ui->lb_y->setText(QString::number(rect.y()));
    ui->lb_w->setText(QString::number(rect.width()));
    ui->lb_h->setText(QString::number(rect.height()));
    update();
}








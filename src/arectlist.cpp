#include "arectlist.h"
#include "ui_ARectList.h"

ARectList::ARectList(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ARectList) {
    ui->setupUi(this);
    setMinimumWidth(200);
    setMaximumWidth(300);
}

ARectList::~ARectList() {

}

ARectListItem *ARectList::add_item(const QString &name, const QRectF &rect) {

    auto rect_list_item = new ARectListItem();
    rect_list_item->set_item(name, rect);
    ui->verticalLayout->addWidget(rect_list_item);
    return rect_list_item;
}

void ARectList::clear() {
    QLayoutItem *item;
    while ((item = this->layout()->takeAt(0)) != nullptr) {
        delete item->widget();  // 删除控件
        delete item;            // 删除布局项
    }
}







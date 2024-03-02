#include "itemlist.h"
#include "ui_itemlist.h"

ItemList::ItemList(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ItemList) {
    ui->setupUi(this);
}

ItemList::~ItemList() {
    delete ui;
}

QListWidget *ItemList::get_list_widget() {
    return ui->lw_rectCoordinate;
}

QLabel *ItemList::get_label() {
    return ui->lb_rectTextResult;
}

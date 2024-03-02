#include "itemlist.h"
#include "ui_itemlist.h"

ItemList::ItemList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemList)
{
    ui->setupUi(this);
}

ItemList::~ItemList()
{
    delete ui;
}

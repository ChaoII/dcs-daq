#pragma once

#include <QWidget>
#include <QListWidget>
#include <QLabel>

namespace Ui {
    class ItemList;
}

class ItemList : public QWidget {
Q_OBJECT

public:
    explicit ItemList(QWidget *parent = nullptr);

    ~ItemList();

    QListWidget* get_list_widget();

    QLabel* get_label();

private:
    Ui::ItemList *ui;
};



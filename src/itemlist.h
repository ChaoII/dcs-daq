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

private slots:

//    void on_lw_rectCoordinate_itemClicked(QListWidgetItem *item);

//    void on_lw_rectCoordinate_currentRowChanged(int currentRow);


private:
    Ui::ItemList *ui;
};



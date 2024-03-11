#pragma once

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include "arectlistitem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ARectList; }
QT_END_NAMESPACE

class ARectList : public QWidget {
Q_OBJECT

public:
    explicit ARectList(QWidget *parent = nullptr);

    ~ARectList();

    ARectListItem *add_item(const QString &name, const QRectF &rect);

    void clear();

    void re_set_order();

    void clear_item_selected();

protected:
    void mousePressEvent(QMouseEvent *event) override;


signals:

    void item_change_item(ARectListItem *);

private slots:

    void on_receive_item_selected();

private:
    Ui::ARectList *ui;

};



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

    ARectListItem *add_item(const QString &tag_id, const QString &tag_name);

    void clear();

    void re_set_order();

    void update_rect_coordinate(const QRectF &rect);

    void clear_item_selected();

protected:
    void mousePressEvent(QMouseEvent *event) override;


signals:

    void item_change_item(ARectListItem *);

    void item_double_clicked_signal(ARectListItem *);

private slots:

    void on_receive_item_selected();

    void on_double_clicked();

    void on_item_row_changed();

private:
    Ui::ARectList *ui;

};



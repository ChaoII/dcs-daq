//
// Created by AC on 2024/3/11.
//

#pragma once

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>


QT_BEGIN_NAMESPACE
namespace Ui { class ARectListItem; }
QT_END_NAMESPACE

class ARectListItem : public QWidget {
Q_OBJECT

public:
    explicit ARectListItem(QWidget *parent = nullptr);

    ~ARectListItem() override;

    void set_item(const QString &name, const QRectF &rect);

    QLabel *get_order_label();

    void set_selected(bool status);

    bool set_selected_status();

protected:

    void mousePressEvent(QMouseEvent *event) override;

signals:

    void clicked_signal();

private:

    QString last_style_sheet_;

    Ui::ARectListItem *ui;

    bool selected_status_ = false;
};



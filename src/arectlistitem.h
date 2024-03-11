//
// Created by AC on 2024/3/11.
//

#pragma once

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class ARectListItem; }
QT_END_NAMESPACE

class ARectListItem : public QWidget {
Q_OBJECT

public:
    explicit ARectListItem(QWidget *parent = nullptr);

    ~ARectListItem() override;

    void set_item(const QString& name,const QRectF& rect);

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:

    void clicked_signal();

private:
    Ui::ARectListItem *ui;
};



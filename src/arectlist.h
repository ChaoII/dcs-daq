#pragma once

#include <QWidget>
#include <QLabel>
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

protected:


signals:

    void item_change_item(ARectListItem *);

private slots:

private:
    Ui::ARectList *ui;

};



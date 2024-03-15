//
// Created by AC on 2024/3/15.
//

#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ATagInputDialog; }
QT_END_NAMESPACE

class ATagInputDialog : public QDialog {
Q_OBJECT
public:
    explicit ATagInputDialog(QWidget *parent = nullptr, const QString &tag_id = "", const QString &tag_name = "");

    QString get_tag_id();

    QString get_tag_name();


    void set_tag_id(const QString &tag_id);

    void set_tag_name(const QString &tag_name);

    ~ATagInputDialog();

public slots:

    void on_pb_ok_clicked();

    void on_pb_cancel_clicked();

private:
    Ui::ATagInputDialog *ui;


};


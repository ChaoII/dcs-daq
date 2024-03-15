//
// Created by AC on 2024/3/15.
//

#include "ataginputdialog.h"
#include "ui_ATagInputDialog.h"

ATagInputDialog::ATagInputDialog(QWidget *parent, const QString &tag_id, const QString &tag_name) :
        QDialog(parent), ui(new Ui::ATagInputDialog) {
    ui->setupUi(this);
    setWindowTitle("请输入节点id和名称");
    ui->le_tag_id->setText(tag_id);
    ui->le_tag_name->setText(tag_name);
}

QString ATagInputDialog::get_tag_id() {
    return ui->le_tag_id->text();
}

QString ATagInputDialog::get_tag_name() {
    return ui->le_tag_name->text();
}

void ATagInputDialog::set_tag_id(const QString &tag_id) {
    ui->le_tag_id->setText(tag_id);
}

void ATagInputDialog::set_tag_name(const QString &tag_name) {
    ui->le_tag_name->setText(tag_name);
}

void ATagInputDialog::on_pb_ok_clicked() {
    accept();
}

void ATagInputDialog::on_pb_cancel_clicked() {
    reject();
}

ATagInputDialog::~ATagInputDialog() {
    delete ui;
}
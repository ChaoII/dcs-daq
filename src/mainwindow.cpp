#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    graphicsView = new AGraphicsView();
    connect(graphicsView,&AGraphicsView::send_position_signal,this,&MainWindow::update_position_label);
    connect(graphicsView,&AGraphicsView::send_draw_final_signal,this,&MainWindow::on_draw_rect_finished);
    status_view_cord = new QLabel("View 坐标：");
    status_view_cord->setMinimumWidth(150);
    status_scene_cord= new QLabel("Scene 坐标：");
    status_scene_cord->setMinimumWidth(150);
    ui->statusBar->addWidget(status_view_cord);
    ui->statusBar->addWidget(status_scene_cord);
    auto *h_spliter = new QSplitter();
    h_spliter->setOrientation(Qt::Horizontal);
    this->centralWidget()->layout()->addWidget(h_spliter);
    item_list = new ItemList();
    h_spliter->addWidget(graphicsView);
    h_spliter->addWidget(item_list);
    ui->rectangleTool->setEnabled(true);
}

void MainWindow::update_position_label(const QPoint& view_position,const QPoint& scene_position){
    status_view_cord->setText(QString::asprintf("View 坐标：%d,%d",
                                                view_position.x(),view_position.y()));
    status_scene_cord->setText(QString::asprintf("Scene 坐标：%d,%d",
                                                 scene_position.x(),scene_position.y()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openfileAction_triggered()
{
    QString file_path = QFileDialog::getOpenFileName(this, "打开文件", "", "Image FIle (*.jpg *.jpeg *.png)");
    QPixmap pix(file_path);
    if(pix.isNull()){
        QMessageBox::warning(this,"警告","文件打开失败，不支持的文件格式，仅支持jpg、png、bmp 格式");
        return;
    }
    graphicsView->add_image_item(pix);
    ui->rectangleTool->setEnabled(true);
}

void MainWindow::on_rectangleTool_triggered(bool checked)
{
    graphicsView->set_draw_shape_status(checked);
}

void MainWindow::on_draw_rect_finished(const QGraphicsRectItem* item) {

    qDebug() << item->rect();
    qDebug() << item->scale();
    qDebug() << item->data(0).toString();
    item_list->get_list_widget()->addItem("zhangfei");
}


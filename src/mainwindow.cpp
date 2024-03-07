#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QCursor>


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {

    ui->setupUi(this);
    graphicsView_ = new AGraphicsView();
    graphicsView_->installEventFilter(this);
    status_view_cord_ = new QLabel("View 坐标：");
    status_view_cord_->setMinimumWidth(150);
    status_scene_cord_ = new QLabel("Scene 坐标：");
    status_scene_cord_->setMinimumWidth(150);
    ui->statusBar->addWidget(status_view_cord_);
    ui->statusBar->addWidget(status_scene_cord_);

    // 其它toolButton 的icon size 可以给出推荐尺寸
    // int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    // QSize icon_size(size, size);
    // ui->toolBar->setIconSize(icon_size);

    auto action_group = new QActionGroup(this);
    action_group->addAction(ui->rectangleTool);
    action_group->addAction(ui->selectTool);
    ui->selectTool->setChecked(true);


    auto *h_spliter = new QSplitter();
    h_spliter->setOrientation(Qt::Horizontal);
    this->centralWidget()->layout()->addWidget(h_spliter);
    item_list_ = new ItemList();
    h_spliter->addWidget(graphicsView_);
    h_spliter->addWidget(item_list_);
    ui->rectangleTool->setEnabled(true);

    connect(graphicsView_, &AGraphicsView::send_position_signal, this, &MainWindow::update_position_label);
    connect(graphicsView_, &AGraphicsView::send_draw_final_signal, this, &MainWindow::on_draw_rect_finished);
    connect(graphicsView_, &AGraphicsView::item_selected_changed_signal, this, &MainWindow::on_item_selected_changed);
    connect(item_list_->get_list_widget(), &QListWidget::itemClicked, this, &MainWindow::on_current_row_change);
}

void MainWindow::update_position_label(const QPoint &view_position, const QPoint &scene_position) {
    status_view_cord_->setText(QString::asprintf("View 坐标：%d,%d",
                                                 view_position.x(), view_position.y()));
    status_scene_cord_->setText(QString::asprintf("Scene 坐标：%d,%d",
                                                  scene_position.x(), scene_position.y()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_openfileAction_triggered() {
    QString file_path = QFileDialog::getOpenFileName(this, "打开文件", "", "Image FIle (*.jpg *.jpeg *.png)");
    QPixmap pix(file_path);
    if (pix.isNull()) {
        QMessageBox::warning(this, "警告", "文件打开失败，不支持的文件格式，仅支持jpg、png、bmp 格式");
        return;
    }
    graphicsView_->add_image_item(pix);
    ui->rectangleTool->setEnabled(true);
}

void MainWindow::on_rectangleTool_triggered() {
    graphicsView_->set_draw_shape_status();
}

void MainWindow::on_selectTool_triggered() {
    graphicsView_->set_select_status();
}

void MainWindow::on_draw_rect_finished(ARectItem *item) {
    auto list_item = new QListWidgetItem(item->get_id());
    item_list_->get_list_widget()->addItem(list_item);
    items_map_.insert(list_item, item);
    qDebug() << "--------" << items_map_.size();
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        for (auto it = items_map_.begin(); it != items_map_.end();) {
            if (it.value()->isSelected()) {
                // notice 删除当前元素的顺序非常重要
                delete it.value();
                delete it.key();
                // 删除并且迭代器后移
                it = items_map_.erase(it);
            } else {
                // 如果不满足条件，移动到下一个元素
                ++it;
            }
        }
    }
}

void MainWindow::on_item_selected_changed() {
    QMap<QListWidgetItem *, QGraphicsItem *>::iterator it;
    for (it = items_map_.begin(); it != items_map_.end(); it++) {
        if (it.value()->isSelected()) {
            it.key()->setSelected(true);
        } else {
            it.key()->setSelected(false);
        }
    }
}

void MainWindow::on_current_row_change(QListWidgetItem *list_item) {
    auto cur_item = items_map_[list_item];
    if (cur_item) {
        cur_item->setSelected(!cur_item->isSelected());
    }
}

void MainWindow::on_clearTool_triggered() {
    for (auto item: items_map_) {
        graphicsView_->remove_item_from_scene(item);
    }
    items_map_.clear();
    item_list_->get_list_widget()->clear();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *e) {

    return QObject::eventFilter(watched, e);
}


void MainWindow::on_scaleDownTool_triggered() {
//    setCursor(Q);
    graphicsView_->scale_down();
}


void MainWindow::on_scaleUpTool_triggered() {
    graphicsView_->scale_up();
}


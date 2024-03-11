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
    action_group->addAction(ui->previewTool);


    auto *h_spliter = new QSplitter();
    h_spliter->setOrientation(Qt::Horizontal);
    this->centralWidget()->layout()->addWidget(h_spliter);

    item_list_ = new ARectList();


    h_spliter->addWidget(graphicsView_);
    h_spliter->addWidget(item_list_);


    image_pro_ = new ACameraPro(this);
    connect(image_pro_, &ACameraPro::send_image_signal, this, [&](const QImage &img) {
        graphicsView_->update_background_image(img);
    });

    connect(graphicsView_, &AGraphicsView::send_position_signal, this, &MainWindow::update_position_label);
    connect(graphicsView_, &AGraphicsView::send_draw_final_signal, this, &MainWindow::on_draw_rect_finished);
    connect(graphicsView_, &AGraphicsView::item_selected_changed_signal, this, &MainWindow::on_item_selected_changed);
    connect(item_list_, &ARectList::item_change_item, this, &MainWindow::on_current_row_change);

    init_widget();
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


void MainWindow::on_rectangleTool_triggered() {
    graphicsView_->set_draw_shape_status();
}

void MainWindow::on_selectTool_triggered() {
    graphicsView_->set_select_status();
    ui->rectangleTool->setEnabled(true);
    ui->clearTool->setEnabled(true);
    image_pro_->stop();
}

void MainWindow::on_draw_rect_finished(ARectItem *item) {

    auto list_item = item_list_->add_item(item->get_id(), item->sceneBoundingRect());

    items_map_.insert(list_item, item);
    item_list_->re_set_order();
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
    QMap<ARectListItem *, QGraphicsItem *>::iterator it;
    for (it = items_map_.begin(); it != items_map_.end(); it++) {
        if (it.value()->isSelected()) {
//            it.key()->setSelected(true);
        } else {
//            it.key()->setSelected(false);
        }
    }
}

void MainWindow::on_current_row_change(ARectListItem *rect_list_item) {
    auto cur_item = items_map_[rect_list_item];
    if (cur_item) {
        cur_item->setSelected(!cur_item->isSelected());
    }
}

void MainWindow::on_clearTool_triggered() {
    for (auto item: items_map_) {
        graphicsView_->remove_item_from_scene(item);
    }
    items_map_.clear();
    item_list_->clear();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *e) {

    return QObject::eventFilter(watched, e);
}

void MainWindow::on_scaleDownTool_triggered() {
    graphicsView_->scale_down();
}

void MainWindow::on_scaleUpTool_triggered() {
    graphicsView_->scale_up();
}

void MainWindow::on_previewTool_triggered() {
    image_pro_->start();
    ui->rectangleTool->setEnabled(false);
    ui->clearTool->setEnabled(false);
}

void MainWindow::init_widget() {
    graphicsView_->add_image_item(QPixmap(QSize(Config::frame_width, Config::frame_height)));
    is_preview_ = true;
    image_pro_->start();
    ui->previewTool->setChecked(true);
    ui->rectangleTool->setEnabled(false);
    ui->clearTool->setEnabled(false);
}



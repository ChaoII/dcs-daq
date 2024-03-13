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

    opc_ = new OPC();


    auto ocr_thread = new PPOCRV4("model", "dict.txt", false, 4);
    ocr_thread->moveToThread(&ocr_thread_);
    connect(&ocr_thread_, &QThread::finished, ocr_thread, &PPOCRV4::deleteLater);
    connect(this, &MainWindow::predict_signal, ocr_thread, &PPOCRV4::predict);

    connect(graphicsView_, &AGraphicsView::send_position_signal, this, &MainWindow::update_position_label);
    connect(graphicsView_, &AGraphicsView::send_draw_final_signal, this, &MainWindow::on_draw_rect_finished);
    connect(graphicsView_, &AGraphicsView::item_selected_changed_signal, this, &MainWindow::on_item_selected_changed);
    connect(graphicsView_, &AGraphicsView::item_changed_signal, this, &MainWindow::on_item_changed);
    connect(graphicsView_, &AGraphicsView::update_image_signal, this, &MainWindow::on_update_image);
    connect(item_list_, &ARectList::item_change_item, this, &MainWindow::on_current_row_change);

    timer = new QTimer(this);
    timer->setInterval(2000);
    connect(timer, &QTimer::timeout, this, &MainWindow::on_ocr_recognize);

    init_widget();
}

void MainWindow::update_position_label(const QPoint &view_position, const QPoint &scene_position) {
    status_view_cord_->setText(QString::asprintf("View 坐标：%d,%d",
                                                 view_position.x(), view_position.y()));
    status_scene_cord_->setText(QString::asprintf("Scene 坐标：%d,%d",
                                                  scene_position.x(), scene_position.y()));
}

MainWindow::~MainWindow() {
    ocr_thread_.quit();
    ocr_thread_.wait();
    delete ui;
}


void MainWindow::on_rectangleTool_triggered() {
    graphicsView_->set_draw_shape_status();
}

void MainWindow::on_selectTool_triggered() {
    graphicsView_->set_select_status();
    ui->rectangleTool->setEnabled(true);
    ui->clearTool->setEnabled(true);
    ui->importTool->setEnabled(true);
    ui->saveTool->setEnabled(true);
    image_pro_->stop();
    timer->stop();
    is_preview_ = false;
    enable_all_rect_item();
}

void MainWindow::on_draw_rect_finished(ARectItem *item) {
    auto list_item = item_list_->add_item(item->get_id(), item->get_inner_rect());
    items_map_.insert(list_item, item);
    item_list_->re_set_order();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (!is_preview_ && event->key() == Qt::Key_Delete) {
        for (auto it = items_map_.begin(); it != items_map_.end();) {
            if (it.value()->isSelected()) {
                // notice 删除当前元素的顺序非常重要
                delete it.value();
                delete it.key();
                // 删除并且迭代器后移
                it = items_map_.erase(it);
                item_list_->re_set_order();
            } else {
                // 如果不满足条件，移动到下一个元素
                ++it;
            }
        }
    }
}


void MainWindow::on_item_selected_changed() {
    QMap<ARectListItem *, ARectItem *>::iterator it;
    for (it = items_map_.begin(); it != items_map_.end(); it++) {
        if (it.value()->isSelected()) {
            it.key()->set_selected(true);
        } else {
            it.key()->set_selected(false);
        }
    }
}

void MainWindow::on_current_row_change(ARectListItem *rect_list_item) {
    auto cur_item = items_map_[rect_list_item];
    if (cur_item) {
        cur_item->setSelected(rect_list_item->set_selected_status());
    }
}

void MainWindow::on_clearTool_triggered() {
    clear_label();
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
    timer->start();
    graphicsView_->set_select_status();
    ui->rectangleTool->setEnabled(false);
    ui->clearTool->setEnabled(false);
    ui->importTool->setEnabled(false);
    ui->saveTool->setEnabled(false);
    disable_all_rect_item();
}

void MainWindow::on_saveTool_triggered() {
    json_array_ = image_label_to_json();
    Utils::write_json(json_array_, "label.json");
    QMessageBox::information(this, "提示", "文件[label.json]保存成功");
}

void MainWindow::on_importTool_triggered() {
    auto filename = QFileDialog::getOpenFileName(this, "标签导入", "./", "json(*.json)");
    if (filename.isEmpty()) {
        return;
    }
    auto json_array = Utils::read_json(filename);
    if (json_array.isEmpty()) {
        QMessageBox::warning(this, "警告", "文件格式不正确，请选择正确的label文件");
        return;
    } else {
        auto obj = json_array[0].toObject();
        if (obj.value("name").toString().isEmpty() || obj.value("box").toObject().isEmpty()) {
            QMessageBox::warning(this, "警告", "文件格式不正确，请选择正确的label文件");
            return;
        }
    }
    clear_label();
    json_array_ = json_array;
    for (auto object: json_array_) {
        auto id = object.toObject().value("name").toString();
        auto box = object.toObject().value("box").toObject();
        auto x = box.value("x").toDouble();
        auto y = box.value("y").toDouble();
        auto w = box.value("w").toDouble();
        auto h = box.value("h").toDouble();
        graphicsView_->draw_real_rect(id, QRectF(x, y, w, h));
    }
}

void MainWindow::init_widget() {
    graphicsView_->add_image_item(QPixmap(QSize(Config::frame_width, Config::frame_height)));
    is_preview_ = true;
    image_pro_->start();
    ui->previewTool->setChecked(true);
    ui->rectangleTool->setEnabled(false);
    ui->clearTool->setEnabled(false);
    ui->importTool->setEnabled(false);
    ui->saveTool->setEnabled(false);
    timer->start();
    ocr_thread_.start();

    QTimer::singleShot(2000, [&]() {
        load_outer_label("label.json");
        disable_all_rect_item();
    });
}

void MainWindow::on_item_changed(ARectItem *item) {
    QMap<ARectListItem *, ARectItem *>::iterator it;
    for (it = items_map_.begin(); it != items_map_.end(); it++) {
        if (it.value()->get_id() == item->get_id()) {
            it.key()->update_rect(it.value()->get_inner_rect());
        }
    }
}

QJsonArray MainWindow::image_label_to_json() {
    // 弹出保存文件对话框
    QJsonArray arr_phone;
    QJsonObject obj_root, json_box;
    QMap<ARectListItem *, ARectItem *>::iterator it;
    for (it = items_map_.begin(); it != items_map_.end(); it++) {
        auto rect = it.value()->get_inner_rect();
        obj_root["name"] = it.value()->get_id();
        json_box["x"] = rect.x();
        json_box["y"] = rect.y();
        json_box["w"] = rect.width();
        json_box["h"] = rect.height();
        obj_root["box"] = json_box;
        arr_phone.append(obj_root);
    }
    return arr_phone;
}

void MainWindow::on_update_image(const QImage &img) {
    current_image_ = img;
}

void MainWindow::on_ocr_recognize() {
    if (current_image_.isNull()) return;
    cv::Mat img = Utils::qImageToCvMat(current_image_);
    emit predict_signal(img, json_array_);
}

void MainWindow::disable_all_rect_item() {
    QMap<ARectListItem *, ARectItem *>::iterator it;
    for (it = items_map_.begin(); it != items_map_.end(); it++) {
        it.value()->setEnabled(false);
        it.key()->setEnabled(false);
    }
}

void MainWindow::enable_all_rect_item() {
    QMap<ARectListItem *, ARectItem *>::iterator it;
    for (it = items_map_.begin(); it != items_map_.end(); it++) {
        it.value()->setEnabled(true);
        it.key()->setEnabled(true);
    }
}

void MainWindow::load_outer_label(const QString &file_name) {
    json_array_ = Utils::read_json(file_name);
    for (auto object: json_array_) {
        auto id = object.toObject().value("name").toString();
        auto box = object.toObject().value("box").toObject();
        auto x = box.value("x").toDouble();
        auto y = box.value("y").toDouble();
        auto w = box.value("w").toDouble();
        auto h = box.value("h").toDouble();
        graphicsView_->draw_real_rect(id, QRectF(x, y, w, h));
    }
}

void MainWindow::clear_label() {
    for (auto item: items_map_) {
        graphicsView_->remove_item_from_scene(item);
    }
    items_map_.clear();
    item_list_->clear();
}

void MainWindow::on_action1_triggered() {
    opc_->request_endpoints();
}

void MainWindow::on_action2_triggered() {
    opc_->read_node();
}

void MainWindow::on_action3_triggered() {
    opc_->write_attribute();
}








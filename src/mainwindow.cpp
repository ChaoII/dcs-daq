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
    connect(image_pro_, &ACameraPro::send_image_signal, this, &MainWindow::handle_receive_image);
    opc_ = new OPC();

    auto ocr = new PPOCRV4("model", "dict.txt", false, 4);
    ocr->moveToThread(&ocr_thread_);
    connect(ocr, &PPOCRV4::ocr_recognition_finished_signal, this, &MainWindow::handle_ocr_recognition_finished);
    connect(&ocr_thread_, &QThread::finished, ocr, &PPOCRV4::deleteLater);
    connect(this, &MainWindow::predict_signal, ocr, &PPOCRV4::predict);

    connect(graphicsView_, &AGraphicsView::send_position_signal, this, &MainWindow::update_position_label);
    connect(graphicsView_, &AGraphicsView::send_draw_final_signal, this, &MainWindow::handle_draw_rect_finished);
    connect(graphicsView_, &AGraphicsView::item_selected_changed_signal, this,
            &MainWindow::handle_rect_item_selected_changed);
    connect(graphicsView_, &AGraphicsView::item_changed_signal, this, &MainWindow::handle_item_changed);
    connect(graphicsView_, &AGraphicsView::update_image_signal, this, &MainWindow::handle_update_image);
    connect(item_list_, &ARectList::item_change_item, this, &MainWindow::handle_rect_list_select_row_change);
    connect(item_list_, &ARectList::item_double_clicked_signal, this, &MainWindow::handle_item_double_clicked);

    timer_ = new QTimer(this);
    timer_->setInterval(2000);
    connect(timer_, &QTimer::timeout, this, &MainWindow::handle_ocr_recognize);
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
    set_all_rect_enable(true);
    ui->rectangleTool->setEnabled(true);
    ui->clearTool->setEnabled(true);
    ui->importTool->setEnabled(true);
    ui->saveTool->setEnabled(true);
    image_pro_->stop();
    timer_->stop();
    is_preview_ = false;

}

void MainWindow::handle_draw_rect_finished(ARectItem *item, bool is_manual) {
    QString tag_id = "";
    if (is_manual) {
        tag_id = QInputDialog::getText(this, "请输入标签名称", "tag_id:");
    } else {
        tag_id = item->get_id();
    }
    auto list_item = item_list_->add_item(tag_id, item->get_inner_rect());
    item->set_tag_id(tag_id);
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
    QMainWindow::keyPressEvent(event);
}

void MainWindow::handle_rect_item_selected_changed() {
    QMap<ARectListItem *, ARectItem *>::iterator it;
    for (it = items_map_.begin(); it != items_map_.end(); it++) {
        if (it.value()->isSelected()) {
            it.key()->set_selected(true);
        } else {
            it.key()->set_selected(false);
        }
    }
}

void MainWindow::handle_rect_list_select_row_change(ARectListItem *rect_list_item) {
    auto cur_item = items_map_[rect_list_item];
    if (cur_item) {
        cur_item->setSelected(rect_list_item->set_selected_status());
    }
}

void MainWindow::on_clearTool_triggered() {
    clear_label();
}

void MainWindow::on_scaleDownTool_triggered() {
    graphicsView_->scale_down();
}

void MainWindow::on_scaleUpTool_triggered() {
    graphicsView_->scale_up();
}

void MainWindow::on_previewTool_triggered() {
    timer_->start();
    image_pro_->start();
    graphicsView_->set_select_status();
    ui->rectangleTool->setEnabled(false);
    ui->clearTool->setEnabled(false);
    ui->importTool->setEnabled(false);
    ui->saveTool->setEnabled(false);
    set_all_rect_enable(false);
}

void MainWindow::on_saveTool_triggered() {
    json_array_ = image_label_to_json();
    Utils::write_json(json_array_, "label.json");
    QMessageBox::information(this, "提示", "文件[label.json]保存成功");
    opc_->update_nodes(json_array_);
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
        if (obj.value("tag_id").toString().isEmpty() || obj.value("box").toObject().isEmpty()) {
            QMessageBox::warning(this, "警告", "文件格式不正确，请选择正确的label文件");
            return;
        }
    }
    clear_label();
    update_rect_from_json_array(json_array);

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
    ocr_thread_.start();
    timer_->start();

    QTimer::singleShot(1500, [&]() {
        init_rect_from_outer_label("label.json");
        set_all_rect_enable(false);
        opc_->update_nodes(json_array_);
    });
}

void MainWindow::handle_item_changed(ARectItem *item) {
    QMap<ARectListItem *, ARectItem *>::iterator it;
    for (it = items_map_.begin(); it != items_map_.end(); it++) {
        if (it.value()->get_id() == item->get_id()) {
            it.key()->update_rect(it.value()->get_inner_rect());
        }
    }
}

QJsonArray MainWindow::image_label_to_json() {
    QJsonArray arr_phone;
    QJsonObject obj_root, json_box;
    QMap<ARectListItem *, ARectItem *>::iterator it;
    for (it = items_map_.begin(); it != items_map_.end(); it++) {
        auto rect = it.value()->get_inner_rect();
        obj_root["tag_id"] = it.value()->get_tag_id();
        json_box["x"] = rect.x();
        json_box["y"] = rect.y();
        json_box["w"] = rect.width();
        json_box["h"] = rect.height();
        obj_root["box"] = json_box;
        arr_phone.append(obj_root);
    }
    return arr_phone;
}

void MainWindow::handle_update_image(const QImage &img) {
    current_image_ = img;
}

void MainWindow::handle_ocr_recognize() {
    if (current_image_.isNull()) return;
    cv::Mat img = Utils::qImageToCvMat(current_image_);
    emit predict_signal(img, json_array_);
}

void MainWindow::set_all_rect_enable(bool status) {
    item_list_->setEnabled(status);
    graphicsView_->setEnabled(status);
}

void MainWindow::init_rect_from_outer_label(const QString &file_name) {
    auto json_array = Utils::read_json(file_name);
    update_rect_from_json_array(json_array);
}

void MainWindow::clear_label() {
    for (auto item: items_map_) {
        graphicsView_->remove_item_from_scene(item);
    }
    items_map_.clear();
    item_list_->clear();
}


void MainWindow::handle_ocr_recognition_finished(const QJsonArray &ocr_result) {
    opc_->write_attribute(ocr_result);
}

void MainWindow::handle_item_double_clicked(ARectListItem *item) {
    auto tag_id = QInputDialog::getText(this,
                                        "请输入新的tag_id",
                                        "tag_id:",
                                        QLineEdit::Normal, item->get_tag_id());
    if (!tag_id.isEmpty()) {
        auto rect_item = items_map_[item];
        if (rect_item) {
            item->set_tag_id(tag_id);
            rect_item->set_tag_id(tag_id);
        }
    }
}

void MainWindow::handle_receive_image(const QImage &image) {
    graphicsView_->update_background_image(image);
}

void MainWindow::draw_rect_from_json_array(const QJsonArray &json_array) {
    for (auto object: json_array) {
        auto id = object.toObject().value("name").toString();
        auto box = object.toObject().value("box").toObject();
        auto x = box.value("x").toDouble();
        auto y = box.value("y").toDouble();
        auto w = box.value("w").toDouble();
        auto h = box.value("h").toDouble();
        graphicsView_->draw_real_rect(id, QRectF(x, y, w, h));
    }
}

void MainWindow::update_rect_from_json_array(const QJsonArray &json_array) {
    json_array_ = json_array;
    draw_rect_from_json_array(json_array_);
}








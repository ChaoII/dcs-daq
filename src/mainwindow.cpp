#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QCursor>
#include "ataginputdialog.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {

    ui->setupUi(this);
    this->setWindowTitle("DCSDAQ");
    graphicsView_ = new AGraphicsView();
    graphicsView_->installEventFilter(this);
    status_view_cord_ = new QLabel("View 坐标：");
    status_view_cord_->setMinimumWidth(150);
    status_scene_cord_ = new QLabel("Scene 坐标：");
    status_scene_cord_->setMinimumWidth(150);
    ui->statusBar->addWidget(status_view_cord_);
    ui->statusBar->addWidget(status_scene_cord_);
    ui->statusBar->addPermanentWidget(new QLabel("v1.24.3.2"));

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
    item_list_->show();
    graphicsView_->set_select_status();
    set_all_rect_enable(true);
    ui->rectangleTool->setEnabled(true);
    ui->clearTool->setEnabled(true);
    ui->importTool->setEnabled(true);
    ui->saveTool->setEnabled(true);
    ui->fullscreenTool->setEnabled(false);
    image_pro_->stop();
    timer_->stop();
    is_preview_ = false;

}

void MainWindow::handle_draw_rect_finished(ARectItem *item, bool is_manual) {
    QString tag_id = "";
    QString tag_name = "";
    if (is_manual) {
        auto input_dialog = new ATagInputDialog(this, "", "");
        if (QDialog::Accepted != input_dialog->exec()) {
            return;
        }
        tag_id = input_dialog->get_tag_id();
        tag_name = input_dialog->get_tag_name();
        if (tag_id.isEmpty()) {
            delete item;
            return;
        }
        item->set_tag_id(tag_id);
        item->set_tag_name(tag_name);
    } else {
        tag_id = item->get_tag_id();
        tag_name = item->get_tag_name();
    }
    auto list_item = item_list_->add_item(tag_id, tag_name);
    items_.append({list_item, item});
    item_list_->re_set_order();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (!is_preview_ && event->key() == Qt::Key_Delete) {
        for (auto it = items_.begin(); it != items_.end();) {
            if ((*it).second->isSelected()) {
                delete it->second;
                delete it->first;
                it = items_.erase(it);
                item_list_->re_set_order();
            } else {
                it++;
            }
        }
    } else if (is_preview_ && event->key() == Qt::Key_Escape) {
        show_normal();
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::handle_rect_item_selected_changed() {

    for (auto &item_pair: items_) {
        if (item_pair.second->isSelected()) {
            item_pair.first->set_selected(true);
        } else {
            item_pair.first->set_selected(false);
        }
    }
}

void MainWindow::handle_rect_list_select_row_change(ARectListItem *rect_list_item) {

    for (auto &item_pair: items_) {
        if (item_pair.first == rect_list_item) {
            item_pair.second->setSelected(item_pair.first->get_selected_status());
            if (item_pair.second->isSelected()) {
                item_list_->update_rect_coordinate(item_pair.second->get_inner_rect());
            }
        }
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
    item_list_->clear_item_selected();
    item_list_->hide();
    is_preview_ = true;
    graphicsView_->set_select_status();
    ui->rectangleTool->setEnabled(false);
    ui->clearTool->setEnabled(false);
    ui->importTool->setEnabled(false);
    ui->saveTool->setEnabled(false);
    ui->fullscreenTool->setEnabled(true);
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

void MainWindow::on_fullscreenTool_triggered() {
    if (is_preview_) {
        show_full_screen();
    }
}

void MainWindow::init_widget() {
    graphicsView_->add_image_item(QPixmap(QSize(Config::frame_width, Config::frame_height)));
    on_previewTool_triggered();
    QTimer::singleShot(1500, [&]() {
        init_rect_from_outer_label("label.json");
        set_all_rect_enable(false);
        opc_->update_nodes(json_array_);
    });
    get_scale_ratio();
    show_full_screen();
}

void MainWindow::handle_item_changed(ARectItem *item) {
    item_list_->update_rect_coordinate(item->get_inner_rect());
}

QJsonArray MainWindow::image_label_to_json() {
    QJsonArray arr_phone;
    QJsonObject obj_root, json_box;
    for (auto &item_pair: items_) {
        auto rect = item_pair.second->get_inner_rect();
        obj_root["tag_id"] = item_pair.second->get_tag_id();
        obj_root["tag_name"] = item_pair.second->get_tag_name();
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
    for (auto &item_pair: items_) {
        graphicsView_->remove_item_from_scene(item_pair.second);
    }
    items_.clear();
    item_list_->clear();
}


void MainWindow::handle_ocr_recognition_finished(const QJsonArray &ocr_result) {
    opc_->write_attribute(ocr_result);
}

void MainWindow::handle_item_double_clicked(ARectListItem *item) {
    auto input_dialog = new ATagInputDialog(this, item->get_tag_id(), item->get_tag_name());
    if (QDialog::Accepted != input_dialog->exec()) {
        return;
    }
    auto tag_id = input_dialog->get_tag_id();
    auto tag_name = input_dialog->get_tag_name();

    if (!tag_id.isEmpty()) {
        for (auto &item_pair: items_) {
            if (item_pair.first == item) {
                item_pair.first->set_tag_id(tag_id);
                item_pair.first->set_tag_name(tag_name);
                item_pair.second->set_tag_id(tag_id);
                item_pair.second->set_tag_name(tag_name);
            }
        }
    }
}

void MainWindow::handle_receive_image(const QImage &image) {
    graphicsView_->update_background_image(image);
}

void MainWindow::draw_rect_from_json_array(const QJsonArray &json_array) {
    for (auto object: json_array) {
        auto tag_id = object.toObject().value("tag_id").toString();
        auto tag_name = object.toObject().value("tag_name").toString();
        auto box = object.toObject().value("box").toObject();
        auto x = box.value("x").toDouble();
        auto y = box.value("y").toDouble();
        auto w = box.value("w").toDouble();
        auto h = box.value("h").toDouble();
        graphicsView_->draw_real_rect(tag_id, tag_name, QRectF(x, y, w, h));
    }
}

void MainWindow::update_rect_from_json_array(const QJsonArray &json_array) {
    json_array_ = json_array;
    draw_rect_from_json_array(json_array_);
}

void MainWindow::show_full_screen() {
    if (!is_normal_) return;
    this->ui->toolBar->setVisible(false);
    this->ui->statusBar->setVisible(false);
    this->ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
    graphicsView_->reset_scale();
    graphicsView_->scale(scale_ratio_, scale_ratio_);
    this->showFullScreen();
    is_normal_ = false;
}

void MainWindow::show_normal() {
    if (is_normal_) return;
    this->ui->toolBar->setVisible(true);
    this->ui->statusBar->setVisible(true);
    this->ui->verticalLayout->setContentsMargins(9, 9, 9, 9);

    graphicsView_->scale(1 / scale_ratio_, 1 / scale_ratio_);
    this->showNormal();
    is_normal_ = true;
}

void MainWindow::get_scale_ratio() {
    auto screen = QApplication::primaryScreen()->size();
    auto rate_w = (double) screen.width() / Config::frame_width;
    auto rate_h = (double) screen.height() / Config::frame_height;
    scale_ratio_ = rate_w < rate_h ? rate_w : rate_h;
}










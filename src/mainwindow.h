#pragma once

#include <QMainWindow>
#include <QLabel>
#include "arectlist.h"
#include "agraphicsview.h"
#include "acamerapro.h"
#include "ocr/ppocrv4.h"
#include <QThread>
#include "opc/opc.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

protected:

    void keyPressEvent(QKeyEvent *event) override;

signals:

    void predict_signal(const cv::Mat &image, const QJsonArray &json_array);

public slots:

    void update_position_label(const QPoint &, const QPoint &);

private slots:

    void handle_ocr_recognition_finished(const QJsonArray &);

    void handle_draw_rect_finished(ARectItem *item, bool is_manual);

    void handle_rect_list_select_row_change(ARectListItem *table_item);

    void handle_rect_item_selected_changed();

    void handle_item_double_clicked(ARectListItem *item);

    void handle_item_changed(ARectItem *);

    void handle_update_image(const QImage &img);

    void handle_ocr_recognize();

    void handle_receive_image(const QImage &);

    void on_rectangleTool_triggered();

    void on_clearTool_triggered();

    void on_selectTool_triggered();

    void on_scaleDownTool_triggered();

    void on_scaleUpTool_triggered();

    void on_previewTool_triggered();

    void on_saveTool_triggered();

    void on_importTool_triggered();


private:
    void clear_label();

    void init_widget();

    void update_rect_from_json_array(const QJsonArray &json_array);

    void draw_rect_from_json_array(const QJsonArray &json_array);

    QJsonArray image_label_to_json();

    void init_rect_from_outer_label(const QString &);

    void set_all_rect_enable(bool status);

private:
    Ui::MainWindow *ui;
    QLabel *status_view_cord_ = nullptr;
    QLabel *status_scene_cord_ = nullptr;
    ARectList *item_list_ = nullptr;
    AGraphicsView *graphicsView_ = nullptr;
    QList<QPair<ARectListItem *, ARectItem *>> items_;
    ACameraPro *image_pro_ = nullptr;
    bool is_preview_ = false;
    QThread ocr_thread_;
    QImage current_image_;
    QJsonArray json_array_;
    QTimer *timer_ = nullptr;
    OPC *opc_ = nullptr;
};


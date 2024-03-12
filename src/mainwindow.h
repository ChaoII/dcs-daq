#pragma once

#include <QMainWindow>
#include <QLabel>
#include "arectlist.h"
#include "agraphicsview.h"
#include "acamerapro.h"
#include "ocr/ppocrv4.h"
#include <QThread>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

protected:

    void keyPressEvent(QKeyEvent *event) override;

    bool eventFilter(QObject *watched, QEvent *e) override;

signals:

    void predict_signal(const cv::Mat &image, const QJsonArray &json_array);

public slots:

    void update_position_label(const QPoint &, const QPoint &);

private slots:

    void on_rectangleTool_triggered();

    void on_draw_rect_finished(ARectItem *item);

    void on_selectTool_triggered();

    void on_current_row_change(ARectListItem *table_item);

    void on_item_selected_changed();

    void on_item_changed(ARectItem *);

    void on_clearTool_triggered();

    void on_scaleDownTool_triggered();

    void on_scaleUpTool_triggered();

    void on_previewTool_triggered();

    void on_update_image(const QImage &img);

    void on_ocr_recognize();

private:

    void clear_all_tools_select();

    void init_widget();

    QJsonArray image_label_to_json();

private:
    Ui::MainWindow *ui;
    QLabel *status_view_cord_ = nullptr;
    QLabel *status_scene_cord_ = nullptr;
    ARectList *item_list_ = nullptr;
    AGraphicsView *graphicsView_ = nullptr;
    QMap<ARectListItem *, ARectItem *> items_map_;
    ACameraPro *image_pro_ = nullptr;
    bool is_preview_ = false;
    QThread ocr_thread_;
    QImage current_image_;
    QJsonArray json_array_;
    QTimer *timer = nullptr;
};


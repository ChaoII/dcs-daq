#pragma once

#include <QLabel>
#include <QThread>
#include <QMainWindow>
#include "opc/opc.h"
#include "arectlist.h"
#include "agraphicsview.h"
#include "acamerapro.h"
#include "ocr/ppocrv4.h"

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

    /// 更新状态栏视图坐标和场景坐标槽函数
    void update_position_label(const QPoint &, const QPoint &);

private slots:

    /// 收到OCR识别完成时的处理槽函数
    void handle_ocr_recognition_finished(const QJsonArray &);

    /// 收到矩形框绘制完成信号时的处理函数
    void handle_draw_rect_finished(ARectItem *item, bool is_manual);

    /// 列表选项改变后的响应槽函数
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

    void on_fullscreenTool_triggered();

private:
    void clear_label();

    void init_widget();

    void update_rect_from_json_array(const QJsonArray &json_array);

    void draw_rect_from_json_array(const QJsonArray &json_array);

    QJsonArray image_label_to_json();

    void init_rect_from_label_json(const QString &);

    void set_all_rect_enable(bool status);

    void show_full_screen();

    void show_normal();

    void setup_scale_ratio();

    void save_tag_table_file(const QString &filename);

private:
    Ui::MainWindow *ui;
    /// 是否处于预览状态标识
    bool is_preview_ = false;
    /// 画面显示状态标识，全屏或正常
    bool is_normal_ = true;
    /// graphicsView 的缩放比例，全屏时记录缩放因子
    double scale_ratio_ = 1.0;
    /// 在标注过程中记录上一次tag_id,提升标注便捷性
    QString last_tag_id_ = "";
    /// 在标注过程中记录上一次tag_name,提升标注便捷性
    QString last_tag_name_ = "";
    /// 状态栏显示视图坐标标签
    QLabel *status_view_cord_ = nullptr;
    /// 状态栏显示场景坐标标签
    QLabel *status_scene_cord_ = nullptr;
    /// 主界面右侧标签列表
    ARectList *item_list_ = nullptr;
    /// 主界面视图窗口
    AGraphicsView *graphicsView_ = nullptr;
    /// 视图项与列表项对应关系
    QList<QPair<ARectListItem *, ARectItem *>> items_;
    /// 相机图像预处理对象
    ACameraPro *image_pro_ = nullptr;
    /// OCR识别线程
    QThread ocr_thread_;
    /// 当前画面中的QImage对象
    QImage current_image_;
    /// 当前画面中label参数对象（用json存储）
    QJsonArray json_array_;
    /// OCR识别触发器，定期触发
    QTimer *timer_ = nullptr;
    /// opc客户端对象
    OPC *opc_ = nullptr;
};


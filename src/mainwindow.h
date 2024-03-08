#pragma once

#include <QMainWindow>
#include <QLabel>
#include "itemlist.h"
#include "agraphicsview.h"
#include "acamerapro.h"

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

public slots:

    void update_position_label(const QPoint &, const QPoint &);

private slots:

    void on_rectangleTool_triggered();

    void on_draw_rect_finished(ARectItem *item);

    void on_selectTool_triggered();

    void on_current_row_change(QListWidgetItem *list_item);

    void on_item_selected_changed();

    void on_clearTool_triggered();

    void on_scaleDownTool_triggered();

    void on_scaleUpTool_triggered();

    void on_previewTool_triggered();

private:

    void clear_all_tools_select();

    void init_widget();

private:
    Ui::MainWindow *ui;
    QLabel *status_view_cord_ = nullptr;
    QLabel *status_scene_cord_ = nullptr;
    ItemList *item_list_ = nullptr;
    AGraphicsView *graphicsView_ = nullptr;
    QMap<QListWidgetItem *, QGraphicsItem *> items_map_;
    ACameraPro *image_pro_ = nullptr;
    bool is_preview_ = false;
};


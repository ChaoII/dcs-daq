//
// Created by AC on 2024/3/4.
//

#pragma

#include <QColor>

class Config {

public:
    // 缩放比例最小值
    inline static double scale_range_min = 0.3;
    // 缩放比例最大值
    inline static double scale_range_max = 4.0;
    // 默认场景大小
    inline static int default_scene_size = 1000;
    // 值越大，滚动鼠标的过程中缩放速度越快
    inline static double scale_step = 1.1;

    inline static QColor view_bg_color = QColor(53, 53, 53);

    inline static QColor view_inter_line_color = QColor(60, 60, 60);

    inline static int inter_grid_step = 15;

    inline static QColor view_border_line_color = QColor(0, 0, 0);

    inline static int border_grid_step = 150;

    inline static int frame_width = 640;

    inline static int frame_height = 480;
};


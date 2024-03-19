#pragma once

#include <QColor>
#include <QDebug>


class Config {
    struct ScaleRange {
        double min;
        double max;
    };
    struct FrameSize {
        int width;
        int height;
    };

public:
    static Config &Instance() {
        static Config config;
        return config;
    };


private:
    explicit Config();

    ~Config() = default;

    Config(const Config &) = delete;

    Config &operator=(const Config &) = delete;

public:
    // 缩放比例区间
    ScaleRange scale_range;
    // 默认场景大小
    int default_scene_size;
    // 值越大，滚动鼠标的过程中缩放速度越快
    double scale_step;
    QColor view_bg_color;
    QColor view_inter_line_color;
    int inter_grid_step;
    int border_grid_step;
    QColor view_border_line_color;
    FrameSize frame_size;
    QString OPC_server_endpoint;
    QString OPC_prefix;
    QString label_json;
    QString tag_table;
    inline static QString version = "v1.24.3.2";
};



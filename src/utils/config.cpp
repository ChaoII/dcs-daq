#include "utils/config.h"
#include "utils/utils.h"

Config::Config() {

    QJsonObject configs = Utils::read_json_object("config.json");
    auto scale_range_obj = configs.value("scale_range").toObject();
    scale_range = {scale_range_obj.value("min").toDouble(), scale_range_obj.value("max").toDouble()};
    // 默认场景大小
    default_scene_size = configs.value("default_scene_size").toInt();
    // 值越大，滚动鼠标的过程中缩放速度越快
    scale_step = configs.value("scale_step").toDouble();
    auto view_bg_color_array = configs.value("view_bg_color").toArray();
    view_bg_color = QColor(view_bg_color_array[0].toInt(), view_bg_color_array[1].toInt(),
                           view_bg_color_array[2].toInt());
    auto view_inter_line_color_array = configs.value("view_inter_line_color").toArray();
    view_inter_line_color = QColor(view_inter_line_color_array[0].toInt(), view_inter_line_color_array[1].toInt(),
                                   view_inter_line_color_array[2].toInt());
    inter_grid_step = configs.value("inter_grid_step").toInt();
    border_grid_step = configs.value("border_grid_step").toInt();
    auto view_border_line_color_array = configs.value("view_border_line_color").toArray();
    view_border_line_color = QColor(view_border_line_color_array[0].toInt(), view_border_line_color_array[1].toInt(),
                                    view_border_line_color_array[2].toInt());
    auto frame_size_obj = configs.value("frame_size").toObject();
    frame_size = {frame_size_obj.value("width").toInt(), frame_size_obj.value("height").toInt()};
    OPC_server_endpoint = configs.value("OPC_server_endpoint").toString();
    OPC_prefix = configs.value("OPC_prefix").toString();
}




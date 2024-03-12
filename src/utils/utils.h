//
// Created by AC on 2024/3/12.
//

#pragma once

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QImage>
#include <opencv2/opencv.hpp>

class Utils {
public:
    static void write_json(const QJsonArray &json_array, const QString &json_file);

    static QJsonArray read_json(const QString &json_file);

    static QImage cvMat2QImage(const cv::Mat &img);

    static cv::Mat qImageToCvMat(const QImage &img);

};



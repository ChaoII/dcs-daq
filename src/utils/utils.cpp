//
// Created by AC on 2024/3/12.
//

#include "utils.h"

void Utils::write_json(const QJsonArray &json_array, const QString &json_file) {

    QJsonDocument json_doc(json_array);
    QByteArray jsonData = json_doc.toJson();
    QFile file(json_file);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonData);
        file.close();
    }
}

QJsonArray Utils::read_json_array(const QString &json_file) {
    QFile file(json_file);
    QByteArray json_data;
    if (!file.open(QIODevice::ReadOnly)) {
        return QJsonArray();
    }
    json_data = file.readAll();
    file.close();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_data);
    if (!json_doc.isArray()) {
        return QJsonArray();
    }
    return json_doc.array();
}

QJsonObject Utils::read_json_object(const QString &json_file) {
    QFile file(json_file);
    QByteArray json_data;
    if (!file.open(QIODevice::ReadOnly)) {
        return QJsonObject();
    }
    json_data = file.readAll();
    file.close();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_data);
    if (!json_doc.isObject()) {
        return QJsonObject();
    }
    return json_doc.object();
}

QImage Utils::cvMat2QImage(const cv::Mat &img) {
    if (img.channels() != 3) {
        qDebug() << "unsupported cv image chich channel is:" << img.channels();
        return {};
    }
    int height = img.rows;
    int width = img.cols;
    int step = img.step;
    cv::Mat dst;
    cv::cvtColor(img, dst, cv::COLOR_BGR2RGB);
    QImage q_img = QImage(dst.data, width, height, step, QImage::Format_RGB888).copy();
    return q_img;
}

cv::Mat Utils::qImageToCvMat(const QImage &img) {
//    if (img.format() != QImage::Format_RGB888) {
//        qDebug() << "QImage only support Format_RGB888,but the QImage format is:" << img.format();
//        return {};
//    }
    auto imgs = img.convertToFormat(QImage::Format_RGB888, Qt::NoFormatConversion);
    auto swap = imgs.rgbSwapped();
    return cv::Mat(swap.height(), swap.width(), CV_8UC3, (unsigned char *) swap.bits(),
                   (size_t) swap.bytesPerLine()).clone();
}

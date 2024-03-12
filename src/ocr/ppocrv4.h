//
// Created by AC on 2024/3/12.
//

#pragma once

#include <QObject>
#include "fastdeploy/vision.h"
#include "utils/utils.h"

using OCRRecognitionModel = fastdeploy::vision::ocr::Recognizer;
using OCRResult = fastdeploy::vision::OCRResult;

class PPOCRV4 : public QObject {
Q_OBJECT
public:
    explicit PPOCRV4(const std::string &rec_model_dir,
                     const std::string &rec_label_file,
                     bool use_gpu,int thread_num);

~PPOCRV4();

public slots:

    void predict(const cv::Mat &image, const QJsonArray &json_array);

private:
    void ocr_result_to_json(const QString &data_id, const OCRResult &result, QJsonArray &root);

private:
    OCRRecognitionModel *model = nullptr;
};



//
// Created by AC on 2024/3/12.
//

#include "ppocrv4.h"

PPOCRV4::PPOCRV4(const std::string &rec_model_dir,
                 const std::string &rec_label_file,
                 bool use_gpu, int thread_num) {
    auto rec_model_file = rec_model_dir + "/" + "inference.pdmodel";
    auto rec_params_file = rec_model_dir + "/" + "inference.pdiparams";
    fastdeploy::SetLogger(false);
    fastdeploy::RuntimeOption option;
    option.UseCpu();
    if (use_gpu) {
        option.UseGpu();
    }
    option.SetCpuThreadNum(thread_num);
    model = new OCRRecognitionModel(
            rec_model_file,
            rec_params_file,
            rec_label_file,
            option);
    assert(model->Initialized());
}


void PPOCRV4::predict(const cv::Mat &image, const QJsonArray &json_array) {
    std::vector<cv::Mat> images;
    QJsonArray results;
    auto start_time = std::chrono::steady_clock::now();
    for (const auto &json_object: json_array) {
        auto name = json_object.toObject().value("tag_id").toString();
        auto box = json_object.toObject().value("box").toObject();
        double x = box["x"].toDouble();
        double y = box["y"].toDouble();
        double w = box["w"].toDouble();
        double h = box["h"].toDouble();
        auto dst_image = image(cv::Rect(x, y, w, h));
        fastdeploy::vision::OCRResult result;
        if (!model->Predict(dst_image, &result)) {
            std::cerr << "Failed to predict." << std::endl;
            continue;
        }
        if (result.text.empty()) {
            qDebug() << "data id: " << name << "in this coordinate: [" << x << " " << y << " " << w << " "
                     << h << "] has not ant text";
            continue;
        }
        ocr_result_to_json(name, result, results);
    }
    Utils::write_json(results, "result.json");
    emit ocr_recognition_finished_signal(results);
    auto end_time = std::chrono::steady_clock::now();
    auto elapsed = end_time - start_time;
    qDebug() << "recognition finished, elapsed: "
             << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
}

void PPOCRV4::ocr_result_to_json(const QString &tag_id, const OCRResult &result, QJsonArray &root) {
    QJsonObject json_result;
    json_result["tag_id"] = tag_id;
    auto tag_value = 100.36;
    if (result.rec_scores[0] > 0.2f) {
        tag_value = QString::fromStdString(result.text[0]).toDouble();
    }
    json_result["text"] = tag_value;
    json_result["score"] = QString::number(result.rec_scores[0]);
    root.append(json_result);
}

PPOCRV4::~PPOCRV4() {
    delete model;
    model = nullptr;
}

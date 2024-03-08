//
// Created by AC on 2024/3/8.
//

#include "acamerapro.h"
#include <QTimer>

ACameraPro::ACameraPro(QObject *parent) : QObject(parent) {
    camera_list_ = QMediaDevices::videoInputs();
    image_capture_ = new QImageCapture;
    capture_session_.setImageCapture(image_capture_);
    camera_ = new QCamera(QMediaDevices::defaultVideoInput(), this);
    capture_session_.setCamera(camera_);

    connect(image_capture_, &QImageCapture::imageCaptured, this, [&](int c, const QImage &img) {
        emit send_image_signal(img);
    });

    timer_image_capture_ = new QTimer(this);
    connect(timer_image_capture_, &QTimer::timeout, this, [&]() {
        image_capture_->capture();
    });
}

//抓拍


QList<QCameraDevice> ACameraPro::get_cameras() {
    return camera_list_;
}

void ACameraPro::start() {
    if (camera_->isAvailable()) {
        camera_->start();
        timer_image_capture_->start(30);
    }
}

void ACameraPro::stop() {
    if (camera_->isAvailable()) {
        camera_->stop();
        timer_image_capture_->stop();
    }
}

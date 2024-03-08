//
// Created by AC on 2024/3/8.
//
#pragma once

#include<QCamera>
#include <QMediaDevices>            //管理摄像头的设备表
#include <QImageCapture>
#include <QMediaRecorder>
#include <QMediaCaptureSession>
#include <QDir>
#include <QTimer>


class ACameraPro : public QObject {
Q_OBJECT
public:
    explicit ACameraPro(QObject *parent = nullptr);

    QList<QCameraDevice> get_cameras();

    void start();

    void stop();

signals:

    void send_image_signal(const QImage &image);

private:
    QList<QCameraDevice> camera_list_;
    QCamera *camera_;
    QScopedPointer<QMediaRecorder> my_mediaRecorder;
    QMediaCaptureSession capture_session_;
    QImageCapture *image_capture_ = nullptr;
    QTimer *timer_image_capture_ = nullptr;
    bool camera_state_;
};



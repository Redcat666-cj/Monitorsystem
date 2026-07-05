#pragma once
#include <QObject>
#include <QTimer>
#include <opencv2/opencv.hpp>

class VideoCaptureModule : public QObject {
    Q_OBJECT

public:
    explicit VideoCaptureModule(QObject *parent = nullptr);
    ~VideoCaptureModule();

    bool openCamera(int deviceId = 0);
    bool openFile(const QString &filePath);
    cv::Mat getFrame();
    void startCapture();
    void stopCapture();
    bool isCapturing() const;
    void setFps(int fps);

signals:
    void frameReady(const cv::Mat &frame);
    void captureStatusChanged(bool running);
    void errorOccurred(const QString &errorMsg);

private slots:
    void onCaptureTimeout();

private:
    cv::VideoCapture m_capture;
    QTimer *m_timer;
    cv::Mat m_currentFrame;
    bool m_isCapturing;
};

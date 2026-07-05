#include "VideoCaptureModule.h"

VideoCaptureModule::VideoCaptureModule(QObject *parent)
    : QObject(parent), m_isCapturing(false)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &VideoCaptureModule::onCaptureTimeout);
    m_timer->setInterval(33); // ~30fps
}

VideoCaptureModule::~VideoCaptureModule()
{
    stopCapture();
    if (m_capture.isOpened()) {
        m_capture.release();
    }
}

bool VideoCaptureModule::openCamera(int deviceId)
{
    if (m_capture.isOpened()) {
        m_capture.release();
    }
    if (!m_capture.open(deviceId, cv::CAP_DSHOW)) {
        emit errorOccurred(QString::fromUtf8("无法打开摄像头: ") + QString::number(deviceId));
        return false;
    }
    m_capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    m_capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    return true;
}

bool VideoCaptureModule::openFile(const QString &filePath)
{
    if (m_capture.isOpened()) {
        m_capture.release();
    }
    if (!m_capture.open(filePath.toStdString())) {
        emit errorOccurred(QString::fromUtf8("无法打开视频文件: ") + filePath);
        return false;
    }
    return true;
}

void VideoCaptureModule::startCapture()
{
    if (!m_capture.isOpened()) {
        emit errorOccurred(QString::fromUtf8("视频源未打开，无法开始采集"));
        return;
    }
    m_isCapturing = true;
    m_timer->start();
    emit captureStatusChanged(true);
}

void VideoCaptureModule::stopCapture()
{
    if (m_timer) {
        m_timer->stop();
    }
    m_isCapturing = false;
    emit captureStatusChanged(false);
}

cv::Mat VideoCaptureModule::getFrame()
{
    return m_currentFrame.clone();
}

bool VideoCaptureModule::isCapturing() const
{
    return m_isCapturing;
}

void VideoCaptureModule::setFps(int fps)
{
    if (fps > 0) {
        m_timer->setInterval(1000 / fps);
    }
}

void VideoCaptureModule::onCaptureTimeout()
{
    if (!m_capture.isOpened()) {
        return;
    }
    cv::Mat frame;
    if (!m_capture.read(frame) || frame.empty()) {
        emit errorOccurred(QString::fromUtf8("读取帧失败"));
        stopCapture();
        return;
    }
    m_currentFrame = frame.clone();
    emit frameReady(m_currentFrame);
}

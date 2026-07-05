#pragma once
#include <QMainWindow>
#include <QListWidgetItem>
#include "VideoCaptureModule.h"
#include "MotionDetector.h"
#include "ObjectTracker.h"
#include "AlarmManager.h"
#include "Mat2QImage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartClicked();
    void onStopClicked();
    void onFrameReady(const cv::Mat &frame);
    void onMethodChanged(int index);
    void onThresholdChanged(int value);
    void onMinAreaChanged(int value);
    void onStayThresholdChanged(int value);
    void onAreaThresholdChanged(int value);
    void onAlarmTriggered(const AlarmRecord &record);
    void onClearLogClicked();
    void onLogItemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    VideoCaptureModule *m_captureModule;
    MotionDetector m_detector;
    ObjectTracker m_tracker;
    AlarmManager *m_alarmManager;
    int m_alarmCount;

    void updateStatusBar(int trackedCount);
};

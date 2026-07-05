#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QPixmap>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_alarmCount(0)
{
    ui->setupUi(this);

    m_captureModule = new VideoCaptureModule(this);
    m_alarmManager = new AlarmManager(this);

    // 信号槽连接 - 视频采集
    connect(m_captureModule, &VideoCaptureModule::frameReady,
            this, &MainWindow::onFrameReady);
    connect(m_captureModule, &VideoCaptureModule::errorOccurred,
            this, [this](const QString &msg) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), msg);
    });

    // 信号槽连接 - 报警
    connect(m_alarmManager, &AlarmManager::alarmTriggered,
            this, &MainWindow::onAlarmTriggered);

    // 信号槽连接 - 控制按钮
    connect(ui->startButton, &QPushButton::clicked,
            this, &MainWindow::onStartClicked);
    connect(ui->stopButton, &QPushButton::clicked,
            this, &MainWindow::onStopClicked);

    // 信号槽连接 - 参数调整
    connect(ui->methodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onMethodChanged);
    connect(ui->thresholdSlider, &QSlider::valueChanged,
            this, &MainWindow::onThresholdChanged);
    connect(ui->minAreaSlider, &QSlider::valueChanged,
            this, &MainWindow::onMinAreaChanged);
    connect(ui->stayThresholdSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onStayThresholdChanged);
    connect(ui->areaThresholdSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onAreaThresholdChanged);

    // 信号槽连接 - 日志操作
    connect(ui->clearLogButton, &QPushButton::clicked,
            this, &MainWindow::onClearLogClicked);
    connect(ui->logListWidget, &QListWidget::itemDoubleClicked,
            this, &MainWindow::onLogItemDoubleClicked);

    // 滑块值变化时更新标签显示
    connect(ui->thresholdSlider, &QSlider::valueChanged,
            this, [this](int val) {
        ui->thresholdLabel->setText(QString::fromUtf8("二值化阈值：%1").arg(val));
    });
    connect(ui->minAreaSlider, &QSlider::valueChanged,
            this, [this](int val) {
        ui->minAreaLabel->setText(QString::fromUtf8("最小目标面积：%1").arg(val));
    });

    // 初始化按钮状态
    ui->stopButton->setEnabled(false);
    ui->statusBar->showMessage(QString::fromUtf8("就绪 - 请点击[开始监控]按钮"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStartClicked()
{
    if (!m_captureModule->openCamera(0)) {
        QMessageBox::warning(this, QString::fromUtf8("错误"),
                             QString::fromUtf8("无法打开摄像头"));
        return;
    }
    m_detector.reset();
    m_tracker.reset();
    m_captureModule->startCapture();
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->statusBar->showMessage(QString::fromUtf8("监控已启动"), 2000);
}

void MainWindow::onStopClicked()
{
    m_captureModule->stopCapture();
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->videoLabel->setText(QString::fromUtf8("监控已停止"));
    ui->statusBar->showMessage(QString::fromUtf8("监控已停止"), 2000);
}

void MainWindow::onFrameReady(const cv::Mat &frame)
{
    if (frame.empty()) return;

    cv::Mat displayFrame = frame.clone();

    // 1. 运动检测
    std::vector<cv::Rect> detections = m_detector.detect(frame);

    // 2. 目标跟踪
    m_tracker.update(detections);
    std::vector<TrackedObject> trackedObjects = m_tracker.getTrackedObjects();

    // 3. 绘制跟踪框
    for (const auto &obj : trackedObjects) {
        if (obj.lostFrames > 0) continue;
        cv::rectangle(displayFrame, obj.box, cv::Scalar(0, 255, 0), 2);
        std::string label = "ID:" + std::to_string(obj.id) +
            " (" + std::to_string(obj.trackedFrames) + "f)";
        cv::putText(displayFrame, label,
                    cv::Point(obj.box.x, obj.box.y - 5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5,
                    cv::Scalar(0, 255, 0), 1);
    }

    // 4. 报警检查
    m_alarmManager->checkAlarm(trackedObjects, displayFrame);

    // 5. 显示画面
    QImage image = matToQImage(displayFrame);
    ui->videoLabel->setPixmap(QPixmap::fromImage(image)
        .scaled(ui->videoLabel->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation));

    // 6. 更新状态栏
    updateStatusBar(static_cast<int>(trackedObjects.size()));
}

void MainWindow::onMethodChanged(int index)
{
    if (index == 0) {
        m_detector.setMethod(DetectMethod::FrameDiff);
    } else {
        m_detector.setMethod(DetectMethod::BackgroundSub);
    }
    m_tracker.reset();
}

void MainWindow::onThresholdChanged(int value)
{
    m_detector.setThreshold(value);
}

void MainWindow::onMinAreaChanged(int value)
{
    m_detector.setMinArea(value);
}

void MainWindow::onStayThresholdChanged(int value)
{
    m_alarmManager->setStayThreshold(value);
}

void MainWindow::onAreaThresholdChanged(int value)
{
    m_alarmManager->setAreaThreshold(value);
}

void MainWindow::onAlarmTriggered(const AlarmRecord &record)
{
    m_alarmCount++;
    QString itemText = QString("[%1] %2")
        .arg(record.time)
        .arg(record.description);
    ui->logListWidget->addItem(itemText);

    // 滚动到最新记录
    ui->logListWidget->scrollToBottom();

    // 弹窗通知
    QMessageBox::information(this, QString::fromUtf8("报警通知"),
                             record.description);

    // 写入日志文件
    QFile file("alarm_log.csv");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << record.time << ","
               << record.imagePath << ","
               << record.description << "\n";
        file.close();
    }
}

void MainWindow::onClearLogClicked()
{
    ui->logListWidget->clear();
    m_alarmManager->clearHistory();
    m_alarmCount = 0;
    ui->statusBar->showMessage(QString::fromUtf8("报警日志已清除"), 2000);
}

void MainWindow::onLogItemDoubleClicked(QListWidgetItem *item)
{
    auto history = m_alarmManager->getHistory();
    int row = ui->logListWidget->row(item);
    if (row >= 0 && row < static_cast<int>(history.size())) {
        QDesktopServices::openUrl(
            QUrl::fromLocalFile(history[row].imagePath));
    }
}

void MainWindow::updateStatusBar(int trackedCount)
{
    QString status = QString::fromUtf8("跟踪目标: %1 | 报警次数: %2")
        .arg(trackedCount)
        .arg(m_alarmCount);
    ui->statusBar->showMessage(status);
}

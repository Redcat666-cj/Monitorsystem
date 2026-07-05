#include "AlarmManager.h"
#include <QDir>

AlarmManager::AlarmManager(QObject *parent)
    : QObject(parent), m_snapshotDir("snapshots"),
      m_stayThreshold(30), m_areaThreshold(30000), m_cooldown(10)
{
    QDir dir;
    if (!dir.exists(m_snapshotDir)) {
        dir.mkpath(m_snapshotDir);
    }
}

AlarmManager::~AlarmManager() {}

void AlarmManager::checkAlarm(const std::vector<TrackedObject> &objects,
                               const cv::Mat &frame)
{
    QDateTime now = QDateTime::currentDateTime();
    if (m_lastAlarmTime.isValid() &&
        m_lastAlarmTime.secsTo(now) < m_cooldown) {
        return;
    }

    for (const auto &obj : objects) {
        if (obj.trackedFrames >= m_stayThreshold) {
            QString imagePath = saveSnapshot(frame);
            AlarmRecord record;
            record.time = now.toString("yyyy-MM-dd HH:mm:ss");
            record.imagePath = imagePath;
            record.description = QString::fromUtf8("目标ID %1 停留 %2 帧，触发停留报警")
                                     .arg(obj.id)
                                     .arg(obj.trackedFrames);
            m_history.push_back(record);
            m_lastAlarmTime = now;
            emit alarmTriggered(record);
            return;
        }

        int area = obj.box.width * obj.box.height;
        if (area >= m_areaThreshold) {
            QString imagePath = saveSnapshot(frame);
            AlarmRecord record;
            record.time = now.toString("yyyy-MM-dd HH:mm:ss");
            record.imagePath = imagePath;
            record.description = QString::fromUtf8("目标ID %1 面积 %2 像素，触发面积报警")
                                     .arg(obj.id)
                                     .arg(area);
            m_history.push_back(record);
            m_lastAlarmTime = now;
            emit alarmTriggered(record);
            return;
        }
    }
}

std::vector<AlarmRecord> AlarmManager::getHistory() const
{
    return m_history;
}

void AlarmManager::clearHistory()
{
    m_history.clear();
}

void AlarmManager::setSnapshotDir(const QString &dir)
{
    m_snapshotDir = dir;
    QDir().mkpath(dir);
}

void AlarmManager::setStayThreshold(int frames)
{
    m_stayThreshold = frames;
}

void AlarmManager::setAreaThreshold(int area)
{
    m_areaThreshold = area;
}

void AlarmManager::setCooldown(int seconds)
{
    m_cooldown = seconds;
}

QString AlarmManager::saveSnapshot(const cv::Mat &frame)
{
    QString timestamp = QDateTime::currentDateTime()
                            .toString("yyyyMMdd_HHmmss_zzz");
    QString filename = QString("%1/alarm_%2.jpg")
                           .arg(m_snapshotDir)
                           .arg(timestamp);
    cv::imwrite(filename.toStdString(), frame);
    return filename;
}

#pragma once
#include <QObject>
#include <QString>
#include <QDateTime>
#include <vector>
#include <opencv2/opencv.hpp>
#include "ObjectTracker.h"

struct AlarmRecord {
    QString time;
    QString imagePath;
    QString description;
};

class AlarmManager : public QObject {
    Q_OBJECT

public:
    explicit AlarmManager(QObject *parent = nullptr);
    ~AlarmManager();

    void checkAlarm(const std::vector<TrackedObject> &objects, const cv::Mat &frame);
    std::vector<AlarmRecord> getHistory() const;
    void clearHistory();
    void setSnapshotDir(const QString &dir);
    void setStayThreshold(int frames);
    void setAreaThreshold(int area);
    void setCooldown(int seconds);

signals:
    void alarmTriggered(const AlarmRecord &record);

private:
    std::vector<AlarmRecord> m_history;
    QString m_snapshotDir;
    int m_stayThreshold;
    int m_areaThreshold;
    int m_cooldown;
    QDateTime m_lastAlarmTime;

    QString saveSnapshot(const cv::Mat &frame);
};

#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

struct TrackedObject {
    int id;
    cv::Rect box;
    int lostFrames;
    int trackedFrames;
};

class ObjectTracker {
public:
    ObjectTracker();
    ~ObjectTracker();

    void update(const std::vector<cv::Rect> &detections);
    std::vector<TrackedObject> getTrackedObjects() const;
    void reset();
    void setMaxLostFrames(int frames);
    void setMaxDistance(int distance);

private:
    std::vector<TrackedObject> m_objects;
    int m_nextId;
    int m_maxLostFrames;
    int m_maxDistance;

    cv::Point2f getCenter(const cv::Rect &rect) const;
    float distance(const cv::Point2f &a, const cv::Point2f &b) const;
};

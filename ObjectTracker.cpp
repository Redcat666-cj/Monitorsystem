#include "ObjectTracker.h"
#include <algorithm>
#include <limits>
#include <cmath>

ObjectTracker::ObjectTracker()
    : m_nextId(0), m_maxLostFrames(15), m_maxDistance(80)
{
}

ObjectTracker::~ObjectTracker() {}

void ObjectTracker::update(const std::vector<cv::Rect> &detections)
{
    if (m_objects.empty()) {
        for (const auto &det : detections) {
            TrackedObject obj;
            obj.id = m_nextId++;
            obj.box = det;
            obj.lostFrames = 0;
            obj.trackedFrames = 1;
            m_objects.push_back(obj);
        }
        return;
    }

    std::vector<bool> detMatched(detections.size(), false);

    for (auto &obj : m_objects) {
        cv::Point2f objCenter = getCenter(obj.box);
        float minDist = std::numeric_limits<float>::max();
        int bestMatch = -1;

        for (size_t i = 0; i < detections.size(); ++i) {
            if (detMatched[i]) continue;
            cv::Point2f detCenter = getCenter(detections[i]);
            float dist = distance(objCenter, detCenter);
            if (dist < minDist && dist < m_maxDistance) {
                minDist = dist;
                bestMatch = static_cast<int>(i);
            }
        }

        if (bestMatch >= 0) {
            obj.box = detections[bestMatch];
            obj.lostFrames = 0;
            obj.trackedFrames++;
            detMatched[bestMatch] = true;
        } else {
            obj.lostFrames++;
        }
    }

    for (size_t i = 0; i < detections.size(); ++i) {
        if (!detMatched[i]) {
            TrackedObject obj;
            obj.id = m_nextId++;
            obj.box = detections[i];
            obj.lostFrames = 0;
            obj.trackedFrames = 1;
            m_objects.push_back(obj);
        }
    }

    m_objects.erase(
        std::remove_if(m_objects.begin(), m_objects.end(),
            [this](const TrackedObject &obj) {
                return obj.lostFrames > m_maxLostFrames;
            }),
        m_objects.end()
    );
}

std::vector<TrackedObject> ObjectTracker::getTrackedObjects() const
{
    return m_objects;
}

void ObjectTracker::reset()
{
    m_objects.clear();
    m_nextId = 0;
}

void ObjectTracker::setMaxLostFrames(int frames)
{
    m_maxLostFrames = frames;
}

void ObjectTracker::setMaxDistance(int distance)
{
    m_maxDistance = distance;
}

cv::Point2f ObjectTracker::getCenter(const cv::Rect &rect) const
{
    return cv::Point2f(rect.x + rect.width / 2.0f,
                       rect.y + rect.height / 2.0f);
}

float ObjectTracker::distance(const cv::Point2f &a, const cv::Point2f &b) const
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

#include "MotionDetector.h"

MotionDetector::MotionDetector()
    : m_method(DetectMethod::FrameDiff), m_threshold(25), m_minArea(500)
{
    m_bgSubtractor = cv::createBackgroundSubtractorMOG2(500, 16.0, false);
}

MotionDetector::~MotionDetector() {}

void MotionDetector::setMethod(DetectMethod method)
{
    if (m_method != method) {
        m_method = method;
        reset();
    }
}

void MotionDetector::setThreshold(int value)
{
    m_threshold = value;
}

void MotionDetector::setMinArea(int area)
{
    m_minArea = area;
}

void MotionDetector::reset()
{
    m_prevFrame.release();
    m_bgSubtractor = cv::createBackgroundSubtractorMOG2(500, 16.0, false);
}

std::vector<cv::Rect> MotionDetector::detect(const cv::Mat &frame)
{
    if (m_method == DetectMethod::FrameDiff) {
        return detectByFrameDiff(frame);
    } else {
        return detectByBackgroundSub(frame);
    }
}

std::vector<cv::Rect> MotionDetector::detectByFrameDiff(const cv::Mat &frame)
{
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);

    if (m_prevFrame.empty()) {
        m_prevFrame = gray.clone();
        return {};
    }

    cv::Mat diff;
    cv::absdiff(gray, m_prevFrame, diff);

    cv::Mat binary;
    cv::threshold(diff, binary, m_threshold, 255, cv::THRESH_BINARY);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(binary, binary, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, kernel);

    m_prevFrame = gray.clone();
    return extractRects(binary);
}

std::vector<cv::Rect> MotionDetector::detectByBackgroundSub(const cv::Mat &frame)
{
    cv::Mat fgMask;
    m_bgSubtractor->apply(frame, fgMask);

    cv::Mat binary;
    cv::threshold(fgMask, binary, m_threshold, 255, cv::THRESH_BINARY);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(binary, binary, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, kernel);

    return extractRects(binary);
}

std::vector<cv::Rect> MotionDetector::extractRects(const cv::Mat &mask)
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy,
                     cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Rect> rects;
    for (const auto &contour : contours) {
        double area = cv::contourArea(contour);
        if (area >= m_minArea) {
            rects.push_back(cv::boundingRect(contour));
        }
    }
    return rects;
}

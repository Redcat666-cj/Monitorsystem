#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

enum class DetectMethod {
    FrameDiff,
    BackgroundSub
};

class MotionDetector {
public:
    MotionDetector();
    ~MotionDetector();

    std::vector<cv::Rect> detect(const cv::Mat &frame);
    void setMethod(DetectMethod method);
    void setThreshold(int value);
    void setMinArea(int area);
    void reset();

private:
    DetectMethod m_method;
    int m_threshold;
    int m_minArea;
    cv::Mat m_prevFrame;
    cv::Ptr<cv::BackgroundSubtractor> m_bgSubtractor;

    std::vector<cv::Rect> detectByFrameDiff(const cv::Mat &frame);
    std::vector<cv::Rect> detectByBackgroundSub(const cv::Mat &frame);
    std::vector<cv::Rect> extractRects(const cv::Mat &mask);
};

#pragma once
#include <QImage>
#include <opencv2/opencv.hpp>

inline QImage matToQImage(const cv::Mat &mat)
{
    if (mat.empty()) {
        return QImage();
    }

    cv::Mat rgb;
    cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);

    QImage image(rgb.data, rgb.cols, rgb.rows,
                 static_cast<int>(rgb.step),
                 QImage::Format_RGB888);
    return image.copy();
}

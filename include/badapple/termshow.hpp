#ifndef _BADAPPLE_TPRINT_HPP_
#define _BADAPPLE_TPRINT_HPP_
#include <opencv2/opencv.hpp>
namespace term {
  void termshow(const cv::Mat& frame, uint32_t width, uint32_t height);
}
#endif
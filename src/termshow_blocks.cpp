#include <fmt/core.h>
#include <array>
#include <codecvt>
#include <cstdint>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include "badapple/loop.hpp"
#include <badapple/termshow.hpp>
#include <badapple/termutils.hpp>
#include <opencv2/opencv.hpp>

using std::string, std::string_view, std::cout, std::clog;
using namespace std::string_literals;
namespace {
  bool check_in_bounds(const cv::Mat& frame, size_t x, size_t y) {
    return (x < frame.cols) & (y < frame.rows);
  }
  string block_char(const cv::Mat& frame, size_t block_x, size_t block_y) {
    cv::Vec3i a = check_in_bounds(frame, block_x, block_y + 0) ?
      frame.at<cv::Vec3b>(block_y + 0, block_x) :
      cv::Vec3b(0, 0, 0);
    cv::Vec3i b = check_in_bounds(frame, block_x, block_y + 1) ?
      frame.at<cv::Vec3b>(block_y + 1, block_x) :
      cv::Vec3b(0, 0, 0);

    return fmt::format(
      "\e[38;2;{};{};{}m\e[48;2;{};{};{}m▀", a[2], a[1], a[0], b[2], b[1],
      b[0]);
  }
  thread_local string termshow_fb;
}  // namespace

namespace term {
  void termshow(const cv::Mat& frame, uint32_t width, uint32_t height) {
    // std::cerr << type2str(frame.type()) << "\n";
    cv::Mat frame2;

    double fwidth  = frame.cols;
    double fheight = frame.rows;
    double swidth  = width;
    double sheight = height * 2;

    double scale_factor = (swidth / sheight > fwidth / fheight) ?
      (sheight / fheight) :
      (swidth / fwidth);
    int interp_flag = (swidth < fwidth || sheight < fheight) ? cv::INTER_AREA :
                                                               cv::INTER_LINEAR;

    cv::resize(
      frame, frame2, cv::Size(), scale_factor, scale_factor, interp_flag);

    term::move_to(1, 1);

    termshow_fb.clear();
    for (size_t line = 0; line < height; line++) {
      for (size_t col = 0; col < width; col++) {
        size_t block_x = col;
        size_t block_y = line * 2;

        termshow_fb.append(block_char(frame2, block_x, block_y));
      }
      if (line + 1 < height) {
        termshow_fb.push_back('\n');
      }
    }
    cout << termshow_fb;
  }
}  // namespace term
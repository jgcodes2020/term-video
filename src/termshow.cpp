#include <array>
#include <codecvt>
#include <cstdint>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <badapple/termshow.hpp>
#include <badapple/termutils.hpp>
#include <opencv2/opencv.hpp>

using std::string, std::string_view, std::cout, std::clog;
using namespace std::string_literals;

namespace {
  string type2str(int type) {
    string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch (depth) {
      case CV_8U:
        r = "8U";
        break;
      case CV_8S:
        r = "8S";
        break;
      case CV_16U:
        r = "16U";
        break;
      case CV_16S:
        r = "16S";
        break;
      case CV_32S:
        r = "32S";
        break;
      case CV_32F:
        r = "32F";
        break;
      case CV_64F:
        r = "64F";
        break;
      default:
        r = "User";
        break;
    }

    r += "C";
    r += (chans + '0');

    return r;
  }

  string block_char(
    const cv::Mat& bw_frame, const cv::Mat& cl_frame, size_t block_x,
    size_t block_y) {
    // We take advantage of the Braille charset's ordering:
    // The lower 8 bits are a bitmask of the 8 dots of the character.
    char32_t codepoint = 0x2800;

    // whether the specified x-offset is in the image bounds
    std::array<bool, 2> xf = {
      (block_x + 0) <= bw_frame.cols, (block_x + 1) <= bw_frame.cols};
    // whether the specified y-offset is in the image bounds
    std::array<bool, 4> yf = {
      (block_y + 0) <= bw_frame.rows,
      (block_y + 1) <= bw_frame.rows,
      (block_y + 2) <= bw_frame.rows,
      (block_y + 3) <= bw_frame.rows,
    };
    std::array<uchar, 8> b_pixels = {
      bw_frame.at<uchar>(block_y + 0, block_x + 0) > 0x00,
      bw_frame.at<uchar>(block_y + 1, block_x + 0) > 0x00,
      bw_frame.at<uchar>(block_y + 2, block_x + 0) > 0x00,
      bw_frame.at<uchar>(block_y + 0, block_x + 1) > 0x00,
      bw_frame.at<uchar>(block_y + 1, block_x + 1) > 0x00,
      bw_frame.at<uchar>(block_y + 2, block_x + 1) > 0x00,
      bw_frame.at<uchar>(block_y + 3, block_x + 0) > 0x00,
      bw_frame.at<uchar>(block_y + 3, block_x + 1) > 0x00};
    std::array<uint32_t, 8> c_pixels {
      cl_frame.at<uchar>(block_y + 0, block_x + 0),
      cl_frame.at<uchar>(block_y + 1, block_x + 0),
      cl_frame.at<uchar>(block_y + 2, block_x + 0),
      cl_frame.at<uchar>(block_y + 0, block_x + 1),
      cl_frame.at<uchar>(block_y + 1, block_x + 1),
      cl_frame.at<uchar>(block_y + 2, block_x + 1),
      cl_frame.at<uchar>(block_y + 3, block_x + 0),
      cl_frame.at<uchar>(block_y + 3, block_x + 1)};
    // Optimized character encoding.
    // In UTF-8, Braille characters will be encoded as:
    // 1110 0010 1010 00xx 10xx xxxx`
    string block = "\xE2\xA0\x80"s;
    {
      block[1] |= b_pixels[7] << 1;
      block[1] |= b_pixels[6] << 0;
      block[2] |= b_pixels[5] << 5;
      block[2] |= b_pixels[4] << 4;
      block[2] |= b_pixels[3] << 3;
      block[2] |= b_pixels[2] << 2;
      block[2] |= b_pixels[1] << 1;
      block[2] |= b_pixels[0] << 0;
    }
    
    
    return block;
  }
}  // namespace

namespace term {
  void termshow(const cv::Mat& frame, uint32_t width, uint32_t height) {
    cv::Mat frame2, frame3;

    double fwidth  = frame.cols;
    double fheight = frame.rows;
    double swidth  = width * 2;
    double sheight = height * 4;

    double scale_factor = (swidth / sheight > fwidth / fheight) ?
      (sheight / fheight) :
      (swidth / fwidth);
    int interp_flag = (swidth < fwidth || sheight < fheight) ? cv::INTER_AREA :
                                                               cv::INTER_LINEAR;

    cv::resize(
      frame, frame2, cv::Size(), scale_factor, scale_factor, interp_flag);
    cv::cvtColor(frame2, frame3, cv::COLOR_BGR2GRAY);
    cv::threshold(frame3, frame2, 127, 255, cv::THRESH_BINARY);

    term::move_to(1, 1);

    for (size_t line = 0; line < height; line++) {
      for (size_t col = 0; col < width; col++) {
        size_t block_x = col * 2;
        size_t block_y = line * 4;

        cout << block_char(frame3, frame2, block_x, block_y);
      }
      if (line + 1 < height) {
        cout << "\n";
      }
    }
  }
}  // namespace term
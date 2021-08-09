#include <badapple/termshow.hpp>
#include <badapple/termutils.hpp>
#include <codecvt>
#include <cstdint>
#include <iostream>
#include <locale>
#include <opencv2/core/base.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

using std::string, std::cout, std::clog;

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

string block_char(const cv::Mat &frame, size_t block_x, size_t block_y) {
  clog << "Frame type: " << (frame.type()) << "\n";

  // We take advantage of the Braille charset's ordering:
  // The lower 8 bits are a bitmask of the 8 dots of the character.
  char32_t codepoint = 0x2800;

  bool x0_in = (block_x) <= frame.cols;
  bool x1_in = (block_x + 1) <= frame.cols;
  bool y0_in = (block_y) <= frame.rows;
  bool y1_in = (block_y + 1) <= frame.rows;
  bool y2_in = (block_y + 2) <= frame.rows;
  bool y3_in = (block_y + 3) <= frame.rows;

  if (x0_in && y0_in && frame.at<uint8_t>(block_y, block_x) > 0x00)
    codepoint |= 0x01;
  if (x0_in && y1_in && frame.at<uint8_t>(block_y + 1, block_x) > 0x00)
    codepoint |= 0x02;
  if (x0_in && y2_in && frame.at<uint8_t>(block_y + 2, block_x) > 0x00)
    codepoint |= 0x04;
  if (x1_in && y0_in && frame.at<uint8_t>(block_y, block_x + 1) > 0x00)
    codepoint |= 0x08;
  if (x1_in && y1_in && frame.at<uint8_t>(block_y + 1, block_x + 1) > 0x00)
    codepoint |= 0x10;
  if (x1_in && y2_in && frame.at<uint8_t>(block_y + 2, block_x + 1) > 0x00)
    codepoint |= 0x20;
  if (x0_in && y3_in && frame.at<uint8_t>(block_y + 3, block_x) > 0x00)
    codepoint |= 0x40;
  if (x1_in && y3_in && frame.at<uint8_t>(block_y + 3, block_x + 1) > 0x00)
    codepoint |= 0x80;

  // Manual UTF-8 encoding of the codepoint
  static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> encoder;
  return encoder.to_bytes(&codepoint, &codepoint + 1);
}
} // namespace

namespace term {
void termshow(const cv::Mat &frame, uint32_t width, uint32_t height) {

  cv::Mat frame2, frame3;

  double fwidth = frame.cols;
  double fheight = frame.rows;
  double swidth = width * 2;
  double sheight = height * 4;

  double scale_factor = (swidth / sheight > fwidth / fheight)
                            ? (sheight / fheight)
                            : (swidth / fwidth);
  int interp_flag = (swidth < fwidth || sheight < fheight) ? cv::INTER_AREA
                                                           : cv::INTER_LINEAR;

  cv::resize(frame, frame2, cv::Size(), scale_factor, scale_factor,
             interp_flag);
  cv::threshold(frame2, frame3, 127, 255, cv::THRESH_BINARY);

  term::move_to(1, 1);

  for (size_t line = 0; line < height; line++) {
    for (size_t col = 0; col < width; col++) {
      size_t block_x = col * 2;
      size_t block_y = line * 4;

      clog << "Drawing block at (" << block_x << ", " << block_y << ")\n";

      cout << block_char(frame3, block_x, block_y);
    }
    if (line + 1 < height) {
      cout << "\n";
    }
  }
}
} // namespace term
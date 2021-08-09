#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <ratio>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <opencv2/opencv.hpp>

#include <badapple/termutils.hpp>
#include <badapple/termshow.hpp>

#include <iostream>
#include <ios>
#include <string>
#include <array>
#include <memory>
#include <limits>
#include <type_traits>
#include <utility>
#include <thread>

using std::string, std::cout, std::cin, std::clog;

namespace chr = std::chrono;
using namespace std::literals::chrono_literals;

int main(int argc, char* argv[]) {
  term::smcup smcup;
  // Local to this function, but accessible from SIGWINCH handler
  static std::atomic<uint32_t> term_cwidth;
  static std::atomic<uint32_t> term_cheight;
  
  cv::VideoCapture video(argv[1]);
  cout << "Video dimensions: " << video.get(cv::CAP_PROP_FRAME_WIDTH) << "x";
  cout << video.get(cv::CAP_PROP_FRAME_HEIGHT) << "\n";
  
  {
    winsize s = term::size();
    term_cwidth = s.ws_col;
    term_cheight = s.ws_row;
    
    signal(SIGWINCH, [](int sig) {
      winsize s = term::size();
      term_cwidth = s.ws_col;
      term_cheight = s.ws_row;
    });
  }
  
  
  if (!video.isOpened()) {
    cout << "Could not open video\n";
    exit(-1);
  }
  chr::time_point<chr::steady_clock> time = chr::steady_clock::now();
  long total_frames = 0;
  long frame_count = 0;
  while (true) {
    cv::Mat frame;
    cv::Mat frame2;
    video >> frame;
    
    if (frame.empty()) break;
    
    cv::cvtColor(frame, frame2, cv::COLOR_BGR2GRAY);
    
    term::termshow(frame2, term_cwidth, term_cheight);
    
    chr::time_point<chr::steady_clock> time2 = chr::steady_clock::now();
    frame_count++;
    total_frames += chr::duration_cast<chr::milliseconds>(time2 - time).count();
  }
  term::clear();
  cout << "Avg. frame time: " << (total_frames / frame_count) << "ms";
  cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  cv::destroyAllWindows();
}
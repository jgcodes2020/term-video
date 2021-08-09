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
#include <future>

using std::string, std::cout, std::cin, std::clog;

namespace chr = std::chrono;
using namespace std::literals::chrono_literals;
namespace this_thread = std::this_thread;

int main(int argc, char* argv[]) {
  term::setup smcup;
  // Flags
  static std::atomic<uint32_t> term_cwidth;
  static std::atomic<uint32_t> term_cheight;
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
  
  static std::atomic<bool> run_flag(true);
  
  auto render_thread = std::async(std::launch::async, [&argv]() {
    cv::VideoCapture video(argv[1]);
    cout << "Video dimensions: " << video.get(cv::CAP_PROP_FRAME_WIDTH) << "x";
    cout << video.get(cv::CAP_PROP_FRAME_HEIGHT) << "\n";
    
    if (!video.isOpened()) {
      cout << "Could not open video\n";
      exit(-1);
    }
    chr::time_point<chr::steady_clock> time = chr::steady_clock::now();
    while (run_flag) {
      cv::Mat frame;
      cv::Mat frame2;
      video >> frame;
      
      if (frame.empty()) break;
      
      cv::cvtColor(frame, frame2, cv::COLOR_BGR2GRAY);
      term::termshow(frame2, term_cwidth, term_cheight);
      
      chr::time_point<chr::steady_clock> time2 = chr::steady_clock::now();
      if ((time2 - time) < 33ms) {
        this_thread::sleep_for((time + 33ms) - time2);
      }
      time = chr::steady_clock::now();
    }
  });
  
  while (cin.get() != '\e') {
    this_thread::sleep_for(50ms);
    if (render_thread.wait_for(0ms) == std::future_status::ready) {
      break;
    }
  }
  run_flag = false;
}
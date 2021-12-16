#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ratio>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <badapple/termshow.hpp>
#include <badapple/termutils.hpp>
#include <opencv2/videoio.hpp>

#include <array>
#include <filesystem>
#include <future>
#include <ios>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>

using std::string, std::cout, std::cin, std::clog;

namespace chr = std::chrono;
using namespace std::literals::chrono_literals;
namespace this_thread = std::this_thread;

term::setup _guard_(false);

int main(int argc, char* argv[]) {
  // Flags
  static std::atomic<uint32_t> term_cwidth;
  static std::atomic<uint32_t> term_cheight;
  {
    winsize s    = term::size();
    term_cwidth  = s.ws_col;
    term_cheight = s.ws_row;

    signal(SIGWINCH, [](int sig) {
      winsize s    = term::size();
      term_cwidth  = s.ws_col;
      term_cheight = s.ws_row;
    });
  }
  
  if (argc < 2) {
    return 0;
  }

  static std::atomic<bool> run_flag(true);
  
  std::thread render([&] {
    cv::VideoCapture video {argv[1]};
    auto frame_time = (1000ms / video.get(cv::CAP_PROP_FPS));
    if (!video.isOpened()) return;
    cv::Mat frame;
    auto time = chr::high_resolution_clock::now();
    
    _guard_.enable();
    while (run_flag) {
      video >> frame;
      
      if (frame.empty()) break;
      term::termshow(frame, term_cwidth, term_cheight);
      auto now = chr::high_resolution_clock::now();
      this_thread::sleep_for((time + frame_time) - now);
      time = chr::high_resolution_clock::now();
    }
  });
  
  while (cin.get() != '\033');
  run_flag = false;
  render.join();
  cv::destroyAllWindows();
}

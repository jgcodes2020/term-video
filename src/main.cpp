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

term::setup _guard_;

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
  std::ofstream logfile("cv_buildinfo.txt");
  logfile << cv::getBuildInformation();
  
  if (argc < 2) {
    return 0;
  }

  static std::atomic<bool> run_flag(true);

  auto render_thread = std::async(std::launch::async, [&argv]() {
    if (!std::filesystem::exists(argv[1])) {
      std::cerr << "where video\n";
      run_flag = false;
    }

    cv::VideoCapture video;
    if (!video.open(argv[1], cv::CAP_FFMPEG)) {
      std::cerr << "Can't open video. What's up?\n";
      run_flag = false;
    }

    clog << "Video dimensions: " << video.get(cv::CAP_PROP_FRAME_WIDTH) << "x";
    clog << video.get(cv::CAP_PROP_FRAME_HEIGHT) << "\n";
    chr::time_point<chr::steady_clock> time = chr::steady_clock::now();
    cv::Mat frame;
    while (run_flag) {
      video >> frame;

      if (frame.empty())
        break;
      term::termshow(frame, term_cwidth, term_cheight);

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
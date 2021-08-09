#include <badapple/termutils.hpp>

#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <ostream>


using std::cout, std::flush;

namespace term {
  smcup::smcup() {
    // 1. switch to alternate buffer
    // 2. clear screen
    // 3. clear scrollback
    // 4. reset cursor
    cout << "\e[?1049h\e[2J\e[3J\e[1;1H" << flush;
  }
  smcup::~smcup() {
    // switch to main buffer
    cout << "\e[?1049l" << flush;
  }
  
  void clear() {
    // 1. clear screen
    // 2. clear scrollback
    // 3. reset cursor
    cout << "\e[2J\e[3J\e[1;1H" << flush;
  }
  
  winsize size() {
    winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    return ws;
  }
  
  void move_to(unsigned short row, unsigned short col) {
    cout << "\e[" << row << ";" << col << "H" << flush;
  };
}
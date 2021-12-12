#include <badapple/termutils.hpp>

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <ostream>

using std::cout, std::flush;

namespace term {

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
}  // namespace term
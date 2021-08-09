#include <badapple/termutils.hpp>

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <ostream>


using std::cout, std::flush;

namespace term {
  setup::setup() {
    // 1. switch to alternate buffer
    // 2. clear screen
    // 3. clear scrollback
    // 4. reset cursor
    cout << "\e[?1049h\e[2J\e[3J\e[1;1H" << flush;
    
    // Disable canonical input and echo
    tcgetattr(STDOUT_FILENO, &old);
    struct termios curr = old;
    curr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDOUT_FILENO, TCSANOW, &curr);
  }
  setup::~setup() {
    // switch to main buffer
    cout << "\e[?1049l" << flush;
    // Re-enable canonical input and echo
    tcsetattr(STDOUT_FILENO, TCSANOW, &old);
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
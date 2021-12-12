#ifndef _BADAPPLE_TERMUTILS_HPP_
#define _BADAPPLE_TERMUTILS_HPP_

#include <termios.h>
#include <sys/ioctl.h>
#include <iostream>
#include <unistd.h>

namespace term {
  struct setup {
    termios old;
    bool active;
    
    void enable() {
      // 1. switch to alternate buffer
      // 2. clear screen
      // 3. clear scrollback
      // 4. reset cursor
      std::cout << "\e[?1049h\e[2J\e[3J\e[1;1H" << std::flush;

      // Disable canonical input and echo
      tcgetattr(STDOUT_FILENO, &old);
      struct termios curr = old;
      curr.c_lflag &= ~(ICANON | ECHO);
      tcsetattr(STDOUT_FILENO, TCSANOW, &curr);
      active = true;
    }
    
    setup(bool start_now = true) {
      if (start_now) enable();
    }
    
    ~setup() {
      if (active) {
        // switch to main buffer
        std::cout << "\e[?1049l" << std::flush;
        // Re-enable canonical input and echo
        tcsetattr(STDOUT_FILENO, TCSANOW, &old);
        active = false;
      }
    }
  };
  
  void clear();
  
  winsize size();
  
  void move_to(unsigned short row, unsigned short col);
}
#endif
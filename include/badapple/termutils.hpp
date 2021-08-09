#ifndef _BADAPPLE_TERMUTILS_HPP_
#define _BADAPPLE_TERMUTILS_HPP_

#include <termios.h>
#include <sys/ioctl.h>

namespace term {
  struct setup {
    termios old;
    
    setup();
    ~setup();
  };
  
  void clear();
  
  winsize size();
  
  void move_to(unsigned short row, unsigned short col);
}
#endif
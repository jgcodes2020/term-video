#ifndef _BADAPPLE_TERMUTILS_HPP_
#define _BADAPPLE_TERMUTILS_HPP_

#include <sys/ioctl.h>

namespace term {
  struct smcup {
    smcup();
    ~smcup();
  };
  
  void clear();
  
  winsize size();
  
  void move_to(unsigned short row, unsigned short col);
}
#endif
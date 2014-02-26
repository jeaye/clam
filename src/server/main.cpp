#include <iostream>

#include "core.h"

/* TODO: termbox (ncurses? something C++?) UI */
int main(int const, char ** const)
{
  try
  {
    server::core c;
    c.run();
  }
  catch(std::exception const &e)
  { std::cout << "exception: " << e.what() << std::endl; }
  catch(...)
  { std::cout << "unknown error" << std::endl; }

  std::cout << "server done" << std::endl;
}

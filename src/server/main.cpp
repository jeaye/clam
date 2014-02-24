#include <iostream>

#include "controller.h"

int main(int const, char ** const)
{
  try
  {
    server::controller c;
    c.run();
  }
  catch(std::exception const &e)
  { std::cout << "exception: " << e.what() << std::endl; }
  catch(...)
  { std::cout << "unknown error" << std::endl; }

  std::cout << "server done" << std::endl;
}

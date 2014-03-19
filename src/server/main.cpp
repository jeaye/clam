/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: main.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include <iostream>
#include <thread>
#include <chrono>

#include "core.h"
#include "log/logger.h"

int main(int const, char ** const)
{
  try
  {
    /* TODO: Stream like rendering?
     *    term::stream << term::color::red << "error => " << term::color::off;
     *    term::stream << term::move{ 0,  10 } << "new, indented line";
     *    term::stream << term::move_rel{ 0,  10 } << "new, still indented line";
     *    term::stream.flush(); // present?
     *  window streams?
     *
     *  Titles too long? Cut off to show border
     */ 
    log_system("Creating server core");
    server::core core;
    core.run();
  }
  catch(std::exception const &e)
  { std::cout << "exception: " << e.what() << std::endl; }
  catch(...)
  { std::cout << "unknown error" << std::endl; }

  std::cout << "server done" << std::endl;
}

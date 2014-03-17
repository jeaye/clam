/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: cpu.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include <string>

namespace server
{
  namespace stat
  {
    std::string cpu_bar(size_t const width);
    std::string ram_bar(size_t const width);
  }
}

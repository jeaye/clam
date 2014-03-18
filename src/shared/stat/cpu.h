/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: cpu.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include <string>

namespace shared
{
  namespace stat
  {
    std::string make_bar(size_t const width, float const percent);
    std::string cpu_bar(size_t const width);
    std::string ram_bar(size_t const width);
    float cpu_load();
    float free_ram();
  }
}

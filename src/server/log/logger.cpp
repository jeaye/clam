/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: logger.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include <iomanip>
#include <ctime>
#include <chrono>

#include "logger.h"

namespace server
{
  namespace logging
  {
    std::vector<std::reference_wrapper<std::string const>> chrono_buffer;
    std::map<net::address, std::deque<std::string>> address_buffer;

    void save(net::address const &a, std::string const &log)
    {
      auto const now(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
      std::stringstream ss;
      ss << "[" << std::put_time(std::localtime(&now), "%T") << "]: ";

      if(a == net::address{})
      { ss << "(system) "; }
      else
      { ss << "(" << a << ") "; }
      ss << log << std::endl;

      address_buffer[a].push_back(ss.str());
      chrono_buffer.push_back(address_buffer[a].back());

      /* TODO: Write to file. */
    }
  }
}

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
#include <array>
#include <fstream>

#include "logger.h"

namespace server
{
  namespace logging
  {
    std::vector<std::reference_wrapper<std::string const>> chrono_buffer;
    std::map<net::address, std::deque<std::string>> address_buffer;
    std::string directory{ "log/" };

    void initialize()
    {
      /* TODO: Delete the log directory. */
    }

    void save(net::address const &a, std::string const &log)
    {
      std::stringstream ss;
      auto const now(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
#ifdef __APPLE__
      ss << std::put_time(std::localtime(&now), "[%T] ");
#else
      /* XXX: GCC 4.8 does not have std::put_time -_- */
      std::array<char, 256> buff;
      if(std::strftime(buff.data(), buff.size(), "[%T] ", std::localtime(&now)))
      { ss << buff.data(); }
      else
      { ss << "[error]"; }
#endif

      std::string const addr{ (a == net::address{}) ? "system" : a.to_string() };
      ss << "(" << addr << ") ";
      ss << log << std::endl;

      address_buffer[a].push_back(ss.str());
      chrono_buffer.push_back(address_buffer[a].back());

      std::ofstream ofs(directory + addr, std::ios::app);
      ofs << ss.str();
    }
  }
}

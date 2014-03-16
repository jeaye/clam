/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: logger.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "format.h"
#include "shared/network/socket.h"

#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <functional>

#define log_system(...) \
{ \
  std::stringstream ss; \
  server::logging::variadic_format(ss).print(__VA_ARGS__); \
  server::logging::save(shared::network::address{}, ss.str()); \
}
#define log_worker(addr, ...) \
{ \
  std::stringstream ss; \
  server::logging::variadic_format(ss).print(__VA_ARGS__); \
  server::logging::save((addr), ss.str()); \
}

namespace net = shared::network;

namespace server
{
  namespace logging
  {
    extern std::vector<std::reference_wrapper<std::string const>> chrono_buffer;
    extern std::map<net::address, std::deque<std::string>> address_buffer;

    void save(net::address const &a, std::string const &log);
  }
}

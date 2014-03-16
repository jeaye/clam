/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: ip.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include <string>

namespace shared
{
  namespace network
  {
    namespace ip
    {
      std::string get_internal();
      std::string get_external();
    }
  }
}

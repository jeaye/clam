/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: header.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "message.h"
#include "serialize.h"

namespace shared
{
  namespace protocol
  {
    struct header
    {
      public:
        message msg;
        size_t size;

        static size_t constexpr const max_size{ 16 };
    };

    template <>
    inline std::string serialize<header>(header&& o)
    {
      std::ostringstream oss;
      oss << o.msg << " " << o.size;
      std::string res{ oss.str() };
      if(res.size() > header::max_size)
      { throw std::length_error("Header serialized to too large: " + res); }

      /* The header should always be the same size. */
      auto const padding(header::max_size - res.size());
      for(size_t i{}; i < padding; ++i)
      { res.push_back(' '); }

      return res;
    }

    template <>
    inline header deserialize<header>(std::string const &str)
    {
      if(str.size() != header::max_size)
      { throw std::length_error("Deserialized header is corrupt: '" + str + "'"); }

      std::istringstream iss{ str };
      header hdr;
      iss >> hdr.msg;
      iss >> hdr.size;

      if(!iss)
      { throw std::runtime_error("Failed to parse header: " + str); }

      return hdr;
    }
  }
}

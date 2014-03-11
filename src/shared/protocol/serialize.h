/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: serialize.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include <string>
#include <sstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace shared
{
  namespace protocol
  {
    template <typename T>
    std::string serialize(T&& o)
    {
      std::ostringstream oss;
      boost::archive::text_oarchive oa{ oss };
      oa << std::forward<T>(o);
      return oss.str();
    }

    template <typename T>
    T deserialize(std::string const &str)
    {
      T ret;
      std::istringstream iss{ str };
      boost::archive::text_iarchive ia{ iss };
      ia >> ret;
      return ret;
    }
  }
}

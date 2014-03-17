/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: stat.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "serialize.h"

namespace shared
{
  namespace protocol
  {
    struct ask_stat
    {
      private:
        template<typename Archive>
        void serialize(Archive &, unsigned int const)
        { }
        friend class boost::serialization::access;
    };

    struct tell_stat
    {
      float cpu;
      float ram;

      private:
        template<typename Archive>
        void serialize(Archive &ar, unsigned int const)
        {
          ar & cpu;
          ar & ram;
        }
        friend class boost::serialization::access;
    };
  }
}

/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: header.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

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

      private:
        template<typename Archive>
        void serialize(Archive &ar, unsigned int const)
        {
          ar & msg;
          ar & size;
        }
        friend class boost::serialization::access;
    };

  }
}

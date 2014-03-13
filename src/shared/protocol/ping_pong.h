/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: ping_pong.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "serialize.h"

namespace shared
{
  namespace protocol
  {
    /* TODO: Send time? Calculate ping. */
    struct ping
    {
      private:
        template<typename Archive>
        void serialize(Archive &, unsigned int const)
        {
        }
        friend class boost::serialization::access;
    };
    struct pong
    {
      private:
        template<typename Archive>
        void serialize(Archive &, unsigned int const)
        {
        }
        friend class boost::serialization::access;
    };

  }
}

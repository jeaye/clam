/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: reader.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "../generic_pool.h"
#include "shared/network/socket.h"

#include <map>
#include <memory>

namespace net = shared::network;

namespace server
{
  namespace op
  {
    /* TODO: [de]serialization, notifications */
    class reader
    {
      public:
        reader() = default;
        reader(reader const&) = delete;
        reader(reader &&) = default;
        reader& operator =(reader const&) = delete;
        reader& operator =(reader &&) = default;

        void operator ()(std::map<net::address, std::shared_ptr<worker>> &)
        {
          /* TODO: Iterate workers and read/deserialize data. */
        }

      private:
    };
  }
}

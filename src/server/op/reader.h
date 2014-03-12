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
#include "shared/protocol/message.h"
#include "shared/protocol/serialize.h"
#include "shared/protocol/receiver.h"

#include <map>
#include <memory>

namespace net = shared::network;
namespace proto = shared::protocol;

namespace server
{
  namespace op
  {
    class reader
    {
      public:
        reader() = default;
        reader(reader const&) = delete;
        reader(reader &&) = default;
        reader& operator =(reader const&) = delete;
        reader& operator =(reader &&) = default;

        void operator ()(std::map<net::address, std::shared_ptr<worker>> &workers)
        {
          for(auto const &worker : workers)
          {
            auto const socket(worker.second->get_socket());
            proto::receiver::receive(socket);
          }
          while(proto::pool_t::global().poll());
        }

      private:
    };
  }
}

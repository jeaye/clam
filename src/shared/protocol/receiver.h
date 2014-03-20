/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: reciever.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "../network/socket.h"
#include "message.h"

namespace net = shared::network;

namespace shared
{
  namespace protocol
  {
    namespace receiver
    {
      size_t read_size(array_buffer &arr,
                       std::shared_ptr<net::socket> const sock,
                       size_t const size);

      void receive(std::shared_ptr<net::socket> const sock);
    }
  }
}

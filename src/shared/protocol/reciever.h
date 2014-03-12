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
#include "serialize.h"

namespace shared
{
  namespace protocol
  {
    namespace sender
    {
      void recieve(std::shared_ptr<network::socket> const sock)
      {
        array_buffer arr;
        auto const read(sock->receive(arr.data(), arr.size()));
        if(read)
        {
          /* TODO: deserialize header */
          /* TODO: deserialize body */
          /* TODO: send notification of body */
        }
      }
    }
  }
}

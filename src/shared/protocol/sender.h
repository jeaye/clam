/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: sender.h
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
      template <typename T>
      void send(T &&data, std::shared_ptr<network::socket> const sock)
      {
        std::string const body{ serialize<T>(std::forward<T>(data)) };
        std::string const hdr{ serialize<header>({ message_enum<T>::value, body.size() }) };
        sock->send(hdr.data(), hdr.size());
        sock->send(body.data(), body.size());
      }
    }
  }
}

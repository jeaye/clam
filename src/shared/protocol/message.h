/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: message.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "shared/notif/pool.h"

namespace shared
{
  namespace protocol
  {
    /* All different types of messages. */
    enum class message
    {
      ping, pong
    };
    using pool_t = notif::pool<message>;

    struct header
    {
      message msg;
      size_t size;
    };

    /* TODO ping_pong.h */
    struct ping
    {
    };
    struct pong
    {
    };
  }
}

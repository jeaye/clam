/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: message.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "shared/network/socket.h"
#include "shared/notif/pool.h"
#include "ping_pong.h"

#include <array>

namespace shared
{
  namespace protocol
  {
    /* All different types of messages. */
    enum class message
    {
      ping, pong
    };
    static size_t constexpr max_message_size{ 512 };
    using array_buffer = std::array<char, max_message_size>;
    using pool_t = notif::pool<message>;

    template <typename M>
    struct event
    {
      network::address sender;
      M data;
    };

    /* Conversion to enum from type. */
    template <typename T>
    struct message_enum;

    template <>
    struct message_enum<ping>
    { static message constexpr value{ message::ping }; };

    template <>
    struct message_enum<pong>
    { static message constexpr value{ message::pong }; };

    /* Conversion to type from enum. */
    template <message M>
    struct message_type;

    template <>
    struct message_type<message::ping>
    { using type = ping; };

    template <>
    struct message_type<message::pong>
    { using type = pong; };
  }
}
#include "header.h" /* TODO: ehh, really? */

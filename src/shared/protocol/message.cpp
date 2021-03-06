/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: message.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include <signal.h>

#include "message.h"

namespace shared
{
  namespace protocol
  {
    std::array<std::function<void (net::address const&, std::string const&)>,
               static_cast<size_t>(message::max)> notifiers;

    template <message M>
    static void subscribe()
    {
      using T = typename message_type<M>::type;

      notifiers[static_cast<size_t>(M)] = [](net::address const &a, std::string const &b)
      { pool_t::global().post<event<T>>({ a, deserialize<T>(b) }); };
    }

    void initialize()
    {
      /* TODO: More error/state checking in socket? */
      signal(SIGPIPE, SIG_IGN);

      subscribe<message::ping>();
      subscribe<message::pong>();

      subscribe<message::ask_stat>();
      subscribe<message::tell_stat>();
    }
  }
}

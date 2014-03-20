/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: pinger.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "../worker.h"
#include "shared/protocol/sender.h"

#include <memory>
#include <chrono>
#include <map>

namespace net = shared::network;
namespace proto = shared::protocol;

namespace server
{
  namespace op
  {
    class pinger
    {
      public:
        using time_point_t = std::chrono::system_clock::time_point;
        using timeout_t = std::chrono::milliseconds;

        struct entry
        {
          std::weak_ptr<worker> w;
          /* The last time we pinged. */
          time_point_t last_ping;
          /* The last time we received a pong, if any (reset after each ping or pong). */
          time_point_t last_pong;
        };

        pinger();
        pinger(pinger const&) = delete;
        pinger(pinger &&) = default;
        pinger& operator =(pinger const&) = delete;
        pinger& operator =(pinger &&) = default;

        void operator ()(std::map<net::address, std::shared_ptr<worker>> &workers);

      private:
        void added_worker(worker_added const &wa);
        void ponged(proto::event<proto::pong> const &ev);

        std::map<net::address, entry> m_workers;

        /* XXX: Could be constexpr in C++1y. */
        timeout_t const m_ping_delay{ 5000 };
        timeout_t const m_pong_timeout{ 2000 };
    };
  }
}

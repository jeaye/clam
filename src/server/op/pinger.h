/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: pinger.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "../worker.h"
#include "../generic_pool.h"
#include "shared/protocol/serialize.h"
#include "shared/protocol/sender.h"
#include "../log/logger.h"

#include <memory>
#include <chrono>
#include <map>
#include <functional>
#include <string>
#include <stdexcept>

namespace net = shared::network;
namespace proto = shared::protocol;

namespace server
{
  namespace op
  {
    /* TODO: cpp */
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

        pinger() = delete;
        pinger(timeout_t const timeout)
          : m_pong_timeout(timeout)
        {
          generic_pool_t::global().subscribe<worker_added>(
              std::bind(&pinger::added_worker, this, std::placeholders::_1));
          proto::pool_t::global().subscribe<proto::event<proto::pong>>(
              std::bind(&pinger::ponged, this, std::placeholders::_1));
        }
        pinger(pinger const&) = delete;
        pinger(pinger &&) = default;
        pinger& operator =(pinger const&) = delete;
        pinger& operator =(pinger &&) = default;

        void operator ()(std::map<net::address, std::shared_ptr<worker>> &workers)
        {
          auto const now(std::chrono::system_clock::now());
          for(auto it(m_workers.begin()); it != m_workers.end(); )
          {
            auto const shared(it->second.w.lock());
            if(!shared)
            { m_workers.erase(it++); }
            else
            {
              auto const pong_diff(it->second.last_pong - it->second.last_ping);
              auto const ping_diff(now - it->second.last_ping);

              /* If we're pinging again and we've not been ponged, or the pong timer has elapsed. */
              if(((ping_diff > m_ping_delay) && it->second.last_pong == time_point_t{}) ||
                  (pong_diff > m_pong_timeout))
              {
                /* TODO: Send worker death note first. */
                log_worker(shared->get_address(), "slow ping; removing worker");
                workers.erase(shared->get_address());
              }
              else
              if(ping_diff > m_ping_delay)
              {
                log_worker(shared->get_address(), "ping");

                /* Reset ping and pong. */
                it->second.last_pong = {};
                it->second.last_ping = std::chrono::system_clock::now();
                proto::sender::send(shared::protocol::ping{}, shared->get_socket());
              }
              ++it;
            }
          }
        }

      private:
        void added_worker(worker_added const &wa)
        {
          if(m_workers.find(wa.a) != m_workers.end())
          { throw std::runtime_error("Worker already exists in pinger map"); }

          auto const shared(wa.w.lock());
          m_workers[wa.a] =
          {
            wa.w,
            std::chrono::system_clock::now(), /* last_ping */
            {} /* last_pong */
          };
          proto::sender::send(shared::protocol::ping{}, shared->get_socket());
        }

        void ponged(proto::event<proto::pong> const &ev)
        {
          log_worker(ev.sender, "ponged");
          auto const it(m_workers.find(ev.sender));
          if(it != m_workers.end())
          {
            auto const shared(it->second.w.lock());
            if(shared)
            {
              /* Reset pong timer. */
              it->second.last_pong = std::chrono::system_clock::now();
            }
          }
          else
          { throw std::runtime_error("Ponged from ghost worker"); }
        }

        std::map<net::address, entry> m_workers;
        timeout_t const m_ping_delay{ 5000 };
        timeout_t m_pong_timeout;
    };
  }
}

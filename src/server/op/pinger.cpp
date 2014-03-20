/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: pinger.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "pinger.h"
#include "../generic_pool.h"
#include "shared/protocol/serialize.h"
#include "../log/logger.h"

#include <functional>
#include <string>
#include <stdexcept>

namespace server
{
  namespace op
  {
    pinger::pinger()
    {
      generic_pool_t::global().subscribe<worker_added>(
          std::bind(&pinger::added_worker, this, std::placeholders::_1));
      proto::pool_t::global().subscribe<proto::event<proto::pong>>(
          std::bind(&pinger::ponged, this, std::placeholders::_1));
    }

    void pinger::operator ()(std::map<net::address, std::shared_ptr<worker>> &workers)
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
          else if(ping_diff > m_ping_delay)
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

    void pinger::added_worker(worker_added const &wa)
    {
      /* TODO: What if a worker dies and then comes right back before we remove? */
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

    void pinger::ponged(proto::event<proto::pong> const &ev)
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
      {
        log_system("Ponged from ghost worker: %% with %% total workers", ev.sender, m_workers.size());
        //throw std::runtime_error("Ponged from ghost worker");
      }
    }
  }
}

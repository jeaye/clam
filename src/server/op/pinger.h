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
    class pinger
    {
      public:
        using time_point_t = std::chrono::system_clock::time_point;
        using timeout_t = std::chrono::milliseconds;

        struct entry
        {
          std::weak_ptr<worker> w;
          time_point_t last;
        };

        pinger() = delete;
        pinger(timeout_t const timeout)
          : m_timeout(timeout)
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
          for(auto it(m_workers.cbegin()); it != m_workers.cend(); )
          {
            auto const shared(it->second.w.lock());
            if(!shared)
            { m_workers.erase(it++); }
            else
            {
              auto const diff(now - it->second.last);
              if(diff > m_timeout)
              {
                log_worker(shared->get_address(), "slow ping; removing worker");
                workers.erase(shared->get_address());
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
          proto::sender::send(shared::protocol::ping{}, shared->get_socket());
          m_workers[wa.a] = { wa.w, std::chrono::system_clock::now() };
        }

        void ponged(proto::event<proto::pong> const &ev)
        {
          //log_worker(ev.sender, "ponged");
          auto const it(m_workers.find(ev.sender));
          if(it != m_workers.end())
          {
            auto const shared(it->second.w.lock());
            if(shared)
            {
              it->second.last = std::chrono::system_clock::now();
              proto::sender::send(shared::protocol::ping{}, shared->get_socket());
            }
          }
          else
          { throw std::runtime_error("Pong from ghost worker"); }
        }

        std::map<net::address, entry> m_workers;
        timeout_t m_timeout;
    };
  }
}

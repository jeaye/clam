/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: stat_collector.h
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
    class stat_collector
    {
      public:
        using time_point_t = std::chrono::system_clock::time_point;
        using timeout_t = std::chrono::milliseconds;

        struct entry
        {
          std::weak_ptr<worker> w;
          float cpu;
          float ram;
          time_point_t last_ping;
        };

        stat_collector()
        {
          generic_pool_t::global().subscribe<worker_added>(
              std::bind(&stat_collector::added_worker, this, std::placeholders::_1));
          proto::pool_t::global().subscribe<proto::event<proto::tell_stat>>(
              std::bind(&stat_collector::ponged, this, std::placeholders::_1));
        }
        stat_collector(stat_collector const&) = delete;
        stat_collector(stat_collector &&) = default;
        stat_collector& operator =(stat_collector const&) = delete;
        stat_collector& operator =(stat_collector &&) = default;

        void operator ()(std::map<net::address, std::shared_ptr<worker>> &)
        {
          auto const now(std::chrono::system_clock::now());
          for(auto it(m_workers.begin()); it != m_workers.end(); )
          {
            auto const shared(it->second.w.lock());
            if(!shared)
            { m_workers.erase(it++); }
            else
            {
              auto const diff(now - it->second.last_ping);
              if(diff > m_delay)
              {
                log_worker(shared->get_address(), "asking stats");
                it->second.last_ping = std::chrono::system_clock::now();
                proto::sender::send(shared::protocol::ask_stat{}, shared->get_socket());
              }
              ++it;
            }
          }
        }

        entry operator [](net::address const &a)
        {
          auto const it(m_workers.find(a));
          if(it == m_workers.end())
          {
            log_worker(a, "Worker not found in stat_collector");
            throw std::runtime_error("Unknown worker");
          }
          return it->second;
        }

      private:
        void added_worker(worker_added const &wa)
        {
          if(m_workers.find(wa.a) != m_workers.end())
          { throw std::runtime_error("Worker already exists in stat_collector map"); }

          auto const shared(wa.w.lock());
          m_workers[wa.a] = { wa.w, 0.0f, 0.0f, std::chrono::system_clock::now() };
          proto::sender::send(shared::protocol::ask_stat{}, shared->get_socket());
          log_worker(wa.a, "Asking for stats");
        }

        void ponged(proto::event<proto::tell_stat> const &ev)
        {
          log_worker(ev.sender, "told stat");
          auto const it(m_workers.find(ev.sender));
          if(it != m_workers.end())
          {
            auto const shared(it->second.w.lock());
            if(shared)
            {
              it->second.cpu = ev.data.cpu;
              it->second.ram = ev.data.ram;
            }
          }
          else
          { throw std::runtime_error("tell_stat from ghost worker"); }
        }

        std::map<net::address, entry> m_workers;
        timeout_t const m_delay{ 5000 };
    };
  }
}

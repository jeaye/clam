/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: stat_collector.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "stat_collector.h"
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
    stat_collector::stat_collector()
    {
      generic_pool_t::global().subscribe<worker_added>(
          std::bind(&stat_collector::added_worker, this, std::placeholders::_1));
      proto::pool_t::global().subscribe<proto::event<proto::tell_stat>>(
          std::bind(&stat_collector::ponged, this, std::placeholders::_1));
    }

    void stat_collector::operator ()(std::map<net::address, std::shared_ptr<worker>> &)
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

    stat_collector::entry stat_collector::operator [](net::address const &a)
    {
      auto const it(m_workers.find(a));
      if(it == m_workers.end())
      {
        log_worker(a, "Worker not found in stat_collector");
        throw std::runtime_error("Unknown worker");
      }
      return it->second;
    }

    void stat_collector::added_worker(worker_added const &wa)
    {
      if(m_workers.find(wa.a) != m_workers.end())
      { throw std::runtime_error("Worker already exists in stat_collector map"); }

      auto const shared(wa.w.lock());
      m_workers[wa.a] = { wa.w, 0.0f, 0.0f, std::chrono::system_clock::now() };
      proto::sender::send(shared::protocol::ask_stat{}, shared->get_socket());
      log_worker(wa.a, "Asking for stats");
    }

    void stat_collector::ponged(proto::event<proto::tell_stat> const &ev)
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
  }
}

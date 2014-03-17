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
        struct entry
        {
          std::weak_ptr<worker> w;
          float cpu;
          float ram;
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
          for(auto it(m_workers.cbegin()); it != m_workers.cend(); )
          {
            auto const shared(it->second.w.lock());
            if(!shared)
            { m_workers.erase(it++); }
            else
            {
              /* TODO: work */
              ++it;
            }
          }
        }

      private:
        void added_worker(worker_added const &wa)
        {
          if(m_workers.find(wa.a) != m_workers.end())
          { throw std::runtime_error("Worker already exists in stat_collector map"); }

          auto const shared(wa.w.lock());
          m_workers[wa.a] = { wa.w, 0.0f, 0.0f };
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
              log_worker(ev.sender, "cpu: %% ram: %%", ev.data.cpu, ev.data.ram);
              /* TODO: Timeout before asking again. */
              proto::sender::send(shared::protocol::ask_stat{}, shared->get_socket());
            }
          }
          else
          { throw std::runtime_error("tell_stat from ghost worker"); }
        }

        std::map<net::address, entry> m_workers;
    };
  }
}

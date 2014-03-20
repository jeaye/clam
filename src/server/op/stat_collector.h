/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: stat_collector.h
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

        stat_collector();
        stat_collector(stat_collector const&) = delete;
        stat_collector(stat_collector &&) = default;
        stat_collector& operator =(stat_collector const&) = delete;
        stat_collector& operator =(stat_collector &&) = default;

        void operator ()(std::map<net::address, std::shared_ptr<worker>> &);
        entry operator [](net::address const &a);

      private:
        void added_worker(worker_added const &wa);
        void ponged(proto::event<proto::tell_stat> const &ev);

        std::map<net::address, entry> m_workers;
        timeout_t const m_delay{ 5000 };
    };
  }
}

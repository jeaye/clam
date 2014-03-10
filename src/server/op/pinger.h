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

#include <iostream>
#include <memory>
#include <chrono>
#include <map>
#include <functional>
#include <string>
#include <stdexcept>

namespace net = shared::network;

namespace server
{
  namespace op
  {
    /* TODO: listen for pongs. */
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
                std::cout << "slow ping; removing worker at " << shared->get_address() << std::endl;
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
          std::string const ping{ "ping\r\n" };
          shared->get_socket()->send(ping.c_str(), ping.size());
          m_workers[wa.a] = { wa.w, std::chrono::system_clock::now() };
        }

        std::map<net::address, entry> m_workers;
        timeout_t m_timeout;
    };
  }
}

/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: core.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "shared/network/socket.h"
#include "generic_pool.h"
#include "worker.h"

#include "op/reader.h"
#include "op/pinger.h"

#include <map>
#include <thread>
#include <chrono>
#include <functional>

namespace net = shared::network;

namespace server
{
  class core
  {
    public:
      core()
      {
        proto::initialize();
        m_listener.listen(m_port);

        generic_pool_t::global().subscribe<net::socket::accept_result>(
            std::bind(&core::added_worker, this, std::placeholders::_1));
      }

      void run()
      {
        std::thread const t(std::bind(&core::accept, this));
        while(true)
        {
          while(generic_pool_t::global().poll()) ;

          m_reader(m_workers);
          m_pinger(m_workers);

          std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
      }

      void accept()
      {
        /* Dedicated thread for accepting connections. */
        while(true)
        {
          auto res(m_listener.accept());
          std::cout << "accepted connection from "
                    << res.sender << std::endl;
          generic_pool_t::global().post(res);
        }
      }


    private:
      void added_worker(net::socket::accept_result const &res)
      {
        auto const shared(std::make_shared<worker>(res.sender, res.sock));
        m_workers.emplace(std::make_pair(res.sender, shared));

        generic_pool_t::global().post(worker_added{ res.sender, shared, });
      }

      /* Our connection. */
      static net::port_t constexpr m_port{ 2272 };
      net::socket m_listener;

      /* Only the core should own the workers -- everyone else use weak_ptr. */
      std::map<net::address, std::shared_ptr<worker>> m_workers;

      /* Operators. */
      op::reader m_reader;
      op::pinger m_pinger{ std::chrono::milliseconds(1000) };
  };
}

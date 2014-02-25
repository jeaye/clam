#pragma once

#include "shared/network/socket.h"
#include "generic_pool.h"
#include "worker.h"

/* TODO: op directory for these? */
#include "reader.h"
#include "pinger.h"

#include <map>
#include <thread>
#include <chrono>
#include <functional>

namespace net = shared::network;

namespace server
{
  class controller
  {
    public:
      controller()
        : m_pinger(std::chrono::milliseconds(1000))
      {
        if(!m_listener.listen(m_port))
        { throw std::runtime_error("Failed to open port for listening"); }

        generic_pool_t::global().subscribe<net::socket::accept_result>(
            std::bind(&controller::added_worker, this, std::placeholders::_1));
      }

      void run()
      {
        std::thread const t(std::bind(&controller::accept, this));
        while(true)
        {
          while(generic_pool_t::global().poll()) ;

          m_pinger.update(m_workers);

          std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
      }

      void accept()
      {
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

      static net::port_t constexpr m_port{ 2272 };
      net::socket m_listener;
      std::map<net::address, std::shared_ptr<worker>> m_workers;
      reader m_reader;
      pinger m_pinger;
  };
}

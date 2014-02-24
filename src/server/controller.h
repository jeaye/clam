#pragma once

#include "shared/network/socket.h"
#include "shared/notif/pool.h"

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
      using pool_t = shared::notif::pool<controller>;

      controller()
      {
        if(!m_listener.listen(m_port))
        { throw std::runtime_error("Failed to open port for listening"); }

        pool_t::global().subscribe<net::socket::accept_result>(
            std::bind(&controller::client_added, this, std::placeholders::_1));
      }

      void run()
      {
        std::thread const t(std::bind(&controller::accept, this));
        while(true)
        {
          while(pool_t::global().poll()) ;
          std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
      }

      void accept()
      {
        while(true)
        {
          auto res(m_listener.accept());
          std::cout << "accepted connection from "
                    << res.sender.get_address() << ":"
                    << res.sender.get_port() << std::endl;
          pool_t::global().post(res);
        }
      }


    private:
      void client_added(net::socket::accept_result const &res)
      { m_clients[res.sender] = res.sock; }

      static net::port_t constexpr m_port{ 2272 };
      net::socket m_listener;
      std::map<net::address, std::shared_ptr<net::socket>> m_clients;
  };
}

/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: main.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include <iostream>
#include <string>
#include <array>
#include <thread>
#include <memory>

#include "shared/network/socket.h"
#include "shared/protocol/message.h"
#include "shared/protocol/sender.h"
#include "shared/protocol/receiver.h"
#include "shared/stat/cpu.h"

namespace net = shared::network;
namespace proto = shared::protocol;

int main(int const, char ** const)
{
  try
  {
    proto::initialize();

    auto const sock(std::make_shared<net::socket>());
    sock->connect({ "127.0.0.1", 2272 });

    proto::pool_t::global().subscribe<proto::event<proto::ping>>([&]
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      proto::sender::send(proto::pong{}, sock);
      //std::cout << "pinged" << std::endl;
    });

    proto::pool_t::global().subscribe<proto::event<proto::ask_stat>>([&]
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      proto::sender::send(proto::tell_stat{ shared::stat::cpu_load(), shared::stat::free_ram() }, sock);
      std::cout << "asked stat" << std::endl;
    });

    std::cout << "starting..." << std::endl;
    while(true)
    {
      proto::receiver::receive(sock);

      while(proto::pool_t::global().poll());
      std::this_thread::yield();
    }
  }
  catch(std::exception const &e)
  { std::cout << "exception: " << e.what() << std::endl; }
  catch(...)
  { std::cout << "unknown error" << std::endl; }

  std::cout << "client done" << std::endl;
}

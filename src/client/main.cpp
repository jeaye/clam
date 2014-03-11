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

#include "shared/network/socket.h"

namespace net = shared::network;

int main(int const, char ** const)
{
  try
  {
    net::socket sock;
    sock.connect({ "127.0.0.1", 2272 });
    while(true)
    {
      std::array<char, 512> arr;
      auto const read(sock.receive(arr.data(), arr.size()));
      if(read)
      {
        std::cout << arr.data() << std::flush;
        if(std::string{arr.data()}.find("ping") != std::string::npos)
        { sock.send("pong", 5); }
      }

      std::this_thread::yield();
    }
  }
  catch(std::exception const &e)
  { std::cout << "exception: " << e.what() << std::endl; }
  catch(...)
  { std::cout << "unknown error" << std::endl; }

  std::cout << "client done" << std::endl;
}

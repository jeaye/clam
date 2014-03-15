/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: reciever.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "../network/socket.h"
#include "message.h"
#include "serialize.h"

namespace shared
{
  namespace protocol
  {
    namespace receiver
    {
      inline void receive(std::shared_ptr<network::socket> const sock)
      {
        array_buffer arr;
        auto read(sock->receive(arr.data(), arr.size()));
        auto const sender(sock->get_address());
        ssize_t processed{};
        char *data{ arr.data() };
        if(read)
        {
          //std::cout << "sender: " << sender << std::endl;
          //std::cout << "read: " << read << std::endl;

          while(processed < read)
          {
            /* Header. */
            std::string const hdr_text{ data };
            processed += hdr_text.size() + 1;
            data += hdr_text.size() + 1;

            //std::cout << "hdr size: " << hdr_text.size() << std::endl;
            //std::cout << "header: " << hdr_text << std::endl;

            header const hdr(deserialize<header>(hdr_text));
            if(hdr.size > max_message_size)
            { throw std::out_of_range("Header says message is too large"); }

            /* Body. */
            std::string body_text{ data };
            if(body_text.empty())
            {
              size_t read_body{};
              while(read_body < hdr.size)
              { read_body += sock->receive(arr.data() + read_body, arr.size() - read_body); }
              data = arr.data();
              body_text.assign(data);
            }
            processed += body_text.size() + 1;
            data += body_text.size() + 1;

            //std::cout << "body size: " << body_text.size() << std::endl;
            //std::cout << "body: " << body_text << std::endl;

            auto const msg(static_cast<int>(hdr.msg));
            if(msg < 0)
            { throw std::out_of_range("Negative message type"); }
            else if(msg >= static_cast<int>(notifiers.size()))
            { throw std::out_of_range("Out of bounds message type"); }
            
            notifiers[msg](sender, body_text);
          }
        }
      }
    }
  }
}

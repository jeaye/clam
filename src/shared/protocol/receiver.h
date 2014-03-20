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
      /* TODO: cpp */
      inline size_t read_size(array_buffer &arr, size_t const size)
      {
        size_t read{};
        while(read < size)
        {
          /* TODO: Needs timer? Don't get stuck in here. */
          auto ret(sock->receive(arr.data() + read, size));
          if(ret < 0)
          { break; }
          read += ret;
        }
        return read;
      }

      inline void receive(std::shared_ptr<network::socket> const sock)
      {
        array_buffer arr{};

        /* Deserialize header. */
        auto const read_hdr(arr, read_size);
        if(read_hdr != header::max_size)
        { throw std::length_error("Failed to read header"); }

        std::string const hdr_text{ arr.data() };
        if(hdr_text.size() != read_hdr)
        { throw std::length_error("Header size is " +
                                  std::to_string(hdr_text.size()) +
                                  " not " + std::to_string(read_hdr)); }
        header const hdr(deserialize<header>(hdr_text));
        if(hdr.size > max_message_size)
        { throw std::out_of_range("Header says message is too large"); }

        /* Deserialize body. */
        arr.fill('\0');
        auto const read_body(arr, hdr.size);
        if(read_body != hdr.size)
        { throw std::length_error("Failed to read body"); }

        std::string const body_text{ arr.data() };
        auto const sender(sock->get_address());
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

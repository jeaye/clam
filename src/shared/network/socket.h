/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: socket.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "address.h"

#include <memory>
#include <type_traits>
#include <stdexcept>

namespace shared
{
  namespace network
  {
    class socket
    {
      public:
        socket() = default;
        socket(socket const&) = delete;
        socket(socket &&) = default;
        ~socket();
        socket& operator =(socket const&) = delete;
        socket& operator =(socket &&) = default;

        struct from_result
        {
          /* The address from which we read the data. */
          address const sender;
          /* The number of bytes read. */
          ssize_t const read;
        };

        struct accept_result
        {
          /* The address from which we read the data. */
          address const sender;
          std::shared_ptr<socket> sock;
        };

        /* Initializes the ability to use sockets. */
        static bool initialize();
        /* Performs cleanup on socket-based constructs. */
        static void shutdown();

        /* Attempts to open the socket on the specified port for UDP work. */
        void open(port_t const port);
        /* Attempts to TCP connect to the specified address. */
        void connect(address const &addr);
        /* Listens on the specified port for incoming TCP connections. */
        void listen(port_t const port);
        /* Accepts an incoming TCP connection. */
        accept_result accept();
        
        /* Calculates the address for a TCP connection. */
        address get_address() const;

        /* Disconnects the socket from any open connections. */
        void close() const
        { disconnect(); }
        void disconnect() const;

        /* Attempts to send the specified data over a TCP connection. */
        ssize_t send(void const * const data, size_t const size) const;
        template <typename T>
        ssize_t send(T const &data) const
        {
          assert_pod<T>();
          auto const sent(send(&data, sizeof(data)));
          if(sent != sizeof(data))
          { throw std::runtime_error("Could not send all data"); }
          return sent;
        }

        /* Attempts to send the specifieid data to the specified address (UDP). */
        ssize_t send_to(address const &dest, void const * const data, size_t const size) const;
        template <typename T>
        ssize_t send_to(address const &dest, T const &data) const
        {
          assert_pod<T>();
          auto const sent(send_to(dest, &data, sizeof(data)));
          if(sent != sizeof(data))
          { throw std::runtime_error("Could not send all data"); }
          return sent;
        }

        /* Checks if there is any data waiting to be received from the current TCP connection. */
        ssize_t receive(void * const data, size_t const size) const;
        template <typename T>
        ssize_t receive(T &t) const
        {
          assert_pod<T>();
          auto const read(receive(&t, sizeof(t)));
          if(read != sizeof(t))
          { throw std::runtime_error("Could not read all data"); }
          return read;
        }
        /* Checks if there is any data waiting to be received on the listening socket (TCP/UDP). */
        from_result receive_from(void * const data, size_t const size) const;
        template <typename T>
        from_result receive_from(T &t) const
        {
          assert_pod<T>();
          auto const res(receive_from(&t, sizeof(t)));
          if(res.read != sizeof(t))
          { throw std::runtime_error("Could not read all data"); }
          return res;
        }

        /* Sets the socket checks to an asynchronous peek instead of a synchronous wait. */
        bool set_non_blocking() const;

      private:
        template <typename T>
        static void assert_pod()
        { static_assert(std::is_pod<T>::value, "Type must be POD; otherwise use void* overload"); }

#if PLATFORM == PLATFORM_WINDOWS
        SOCKET m_socket{};
        struct hostent *m_host{};
        SOCKADDR_IN hints{};
#else
        int32_t m_socket{}; /* TODO: unique_ptr with deleter. */
        addrinfo m_hints;
        addrinfo *m_info{};
#endif
    };
  }
}

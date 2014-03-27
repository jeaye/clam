/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: address.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include <cstdint>

/* The supported platforms. */
#define PLATFORM_WINDOWS 1
#define PLATFORM_MAC 2
#define PLATFORM_UNIX 3
#define PLATFORM_ARCADE 0

/* Determine the current platform. */
#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif

/* Direct the preprocessor toward necessary includes. */
#if PLATFORM == PLATFORM_WINDOWS
#ifndef _WINSOCKAPI_
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#endif
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#endif

/* Sockets are built into the standard libraries on Unix-based systems.
* Windows, however, fucking sucks.
*/
#if PLATFORM == PLATFORM_WINDOWS
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <string>
#include <iosfwd>

namespace shared
{
  namespace network
  {
    using port_t = uint32_t;

    class address
    {
      public:
        address();
        address(std::string const &dest, port_t const port);

        static address localhost(port_t const port);

        std::string const& get_address() const;
        void set_address(std::string const &addr);

        port_t get_port() const;
        std::string get_port_str() const
        { return std::to_string(m_port); }
        void set_port(port_t const port);

        /* Retreives the sockaddr_in version of this address descriptor. */
        sockaddr_in const* to_sock_addr_in() const;
        std::string to_string() const;

      private:
        /* The string-based IPV4 version of this address. */
        std::string m_addr;
        port_t m_port{};
        /* The underlying address descriptor. */
        sockaddr_in m_sock_addr;
    };

    bool operator <(address const &lhs, address const &rhs);
    bool operator ==(address const &lhs, address const &rhs);
    std::ostream& operator <<(std::ostream &os, address const &rhs);
  }
}

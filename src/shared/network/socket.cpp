/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: socket.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "socket.h"

#include <sstream>
#include <iostream>
#include <cstring>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

namespace shared
{
  namespace network
  {
    socket::~socket()
    { disconnect(); }

    bool socket::initialize()
    {
#if PLATFORM == PLATFORM_WINDOWS
      /* Request Winsock version 2.2. */
      WSADATA wsaData{};
      return WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR;
#else
      return true;
#endif
    }

    void socket::shutdown()
    {
#if PLATFORM == PLATFORM_WINDOWS
      WSACleanup();
#endif
    }

    void socket::open(port_t const port)
    {
      /* Create a UDP socket. */
      m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

      if(m_socket <= 0)
      { throw std::runtime_error("Failed to open socket"); }

      sockaddr_in address;
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = INADDR_ANY;
      address.sin_port = htons(port);

      /* Tie the socket to the description. */
      if(bind(m_socket, reinterpret_cast<sockaddr const*>(&address), sizeof(sockaddr_in)) != 0)
      { throw std::runtime_error("Failed to bind socket"); }
    }

    void socket::listen(port_t const port)
    {
      m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
      if(m_socket == -1)
      { throw std::runtime_error("Failed to open socket"); }

      int const optval{ 1 };
      if(setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0)
      { throw std::runtime_error("Failed to configure socket"); }

      struct sockaddr_in serv_addr;
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_addr.s_addr = INADDR_ANY;
      serv_addr.sin_port = htons(port);

      /* Tie the socket to the description. */
      if(bind(m_socket, reinterpret_cast<sockaddr const*>(&serv_addr), sizeof(sockaddr_in)) != 0)
      { throw std::runtime_error("Failed to bind socket"); }

      /* TODO: Var this. */
      ::listen(m_socket, 10); /* Allow a queue of 10. */
    }

    socket::accept_result socket::accept()
    {
      struct sockaddr_storage from;
      socklen_t addr_size(sizeof(from));
      int32_t const sock(
          ::accept(m_socket, reinterpret_cast<struct sockaddr*>(&from), &addr_size));
      if(sock == -1)
      { throw std::runtime_error("Failed to accept TCP connection"); }

      /* Extrapolate the string address from the address descriptor. */
      sockaddr_in * const from_in{ reinterpret_cast<sockaddr_in*>(&from) };
      char addr[INET_ADDRSTRLEN]{ 0 };
      inet_ntop(AF_INET, &from_in->sin_addr.s_addr, addr, INET_ADDRSTRLEN);

      /* Build a result summarizing the work. */
      std::shared_ptr<socket> ret{ new socket };
      ret->m_socket = sock;
      return { { { addr }, ntohs(from_in->sin_port) }, std::move(ret) };
    }

    void socket::connect(address const &addr)
    {
      /* Get info on the destination. */
      std::memset(&m_hints, 0, sizeof(m_hints));
      m_hints.ai_family = AF_UNSPEC;
      m_hints.ai_socktype = SOCK_STREAM;
      m_hints.ai_flags = AI_PASSIVE;
      int32_t const ret{ getaddrinfo(addr.get_address().c_str(),
                                     addr.get_port_str().c_str(),
                                     &m_hints, &m_info) };
      if(ret)
      { throw std::runtime_error("Failed to get address info"); }

      m_socket = ::socket(m_info->ai_family, m_info->ai_socktype, m_info->ai_protocol);
      if(m_socket == -1)
      { throw std::runtime_error("Failed to create socket"); }

      /* Attempt to connect to the destination. */
      if(::connect(m_socket, m_info->ai_addr, m_info->ai_addrlen) == -1)
      { throw std::runtime_error("Failed to connect socket"); }
    }

    void socket::disconnect() const
    {
#if PLATFORM == PLATFORM_WINDOWS
      ::closesocket(m_socket);
#else
      ::close(m_socket);
#endif
    }

    ssize_t socket::send(void const * const data, size_t const size) const
    { return ::write(m_socket, data, size); }

    ssize_t socket::send_to(address const &dest, void const * const data, size_t const size) const
    {
      /* Send the specified data. */
      return sendto(m_socket, static_cast<char const*>(data), size,
          0, reinterpret_cast<sockaddr*>(
            const_cast<sockaddr_in*>(dest.to_sock_addr_in())), sizeof(sockaddr_in));
    }

    ssize_t socket::receive(void * const data, size_t const size) const
    { return read(m_socket, data, size); }

    socket::from_result socket::receive_from(void *data, size_t const size) const
    {
#if PLATFORM == PLATFORM_WINDOWS
      using socklen_t = int32_t;
#endif

      /* Specified the address that's being received from. */
      sockaddr_in from;
      socklen_t from_length{ sizeof(from) };

      /* Peek and check if there are any bytes waiting. */
      ssize_t const received_bytes{ ::recvfrom(m_socket, static_cast<char*>(data), size,
                                    0, reinterpret_cast<sockaddr*>(&from), &from_length) };

      if(received_bytes <= 0)
      { return { {}, 0 }; }

      /* Extrapolate the string address from the address descriptor. */
      char addr[INET_ADDRSTRLEN]{ 0 };
      inet_ntop(AF_INET, &from.sin_addr.s_addr, addr, INET_ADDRSTRLEN);

      /* Build a result summarizing the work. */
      return { { { addr }, ntohs(from.sin_port) }, received_bytes };
    }

    bool socket::set_non_blocking() const
    {
      /* Set the socket to perform asynchronous checks instead of synchronous waits. */
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
      int32_t non_blocking{ 1 };
      return (::fcntl(m_socket, F_SETFL, O_NONBLOCK, non_blocking) != -1);
#elif PLATFORM == PLATFORM_WINDOWS
      DWORD non_blocking{ 1 };
      return (::ioctlsocket(m_socket, FIONBIO, &non_blocking) == 0);
#endif
    }
  }
}

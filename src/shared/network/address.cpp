/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: address.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "address.h"

#include <ostream>
#include <sstream>
#include <stdexcept>

namespace shared
{
  namespace network
  {
    address::address() : address("127.0.0.1", 23)
    { }

    address::address(std::string const &addr, port_t const port)
      : m_addr(addr), m_port(port)
    {
      std::fill_n((char*)&m_sock_addr, sizeof(m_sock_addr), 0);

      /* Create a description for the underlying address. */
      m_sock_addr.sin_family = AF_INET;
      inet_pton(AF_INET, m_addr.c_str(), &(m_sock_addr.sin_addr));
      m_sock_addr.sin_port = htons(m_port);
    }

    address address::localhost(port_t const port)
    { return { "127.0.0.1", port }; }

    bool operator <(address const &lhs, address const &rhs)
    {
      /* TODO: optimize */
      bool const lt{ lhs.get_address() < rhs.get_address() };
      if(lt)
      { return true; }
      else if((lhs.get_address() == rhs.get_address()) &&
               lhs.get_port() < rhs.get_port())
      { return true; }
      return false;
    }

    bool operator ==(address const &lhs, address const &rhs)
    {
      return (lhs.get_address() == rhs.get_address()) &&
             (lhs.get_port() == rhs.get_port());
    }

    std::ostream& operator <<(std::ostream &os, address const &rhs)
    { return (os << rhs.get_address() << ":" << rhs.get_port()); }

    std::string const& address::get_address() const
    { return m_addr; }

    void address::set_address(std::string const &addr)
    {
      m_addr = addr;
      ::inet_pton(AF_INET, m_addr.c_str(), &m_sock_addr.sin_addr);
    }

    port_t address::get_port() const
    { return m_port; }

    void address::set_port(port_t const port)
    {
      m_port = port;
      m_sock_addr.sin_port = htons(port);
    }

    sockaddr_in const* address::to_sock_addr_in() const
    { return &m_sock_addr; }

    std::string address::to_string() const
    {
      std::stringstream ss;
      if(ss << *this)
      { return ss.str(); }
      else
      { throw std::runtime_error("Failed to serialize address"); }
    }
  }
}

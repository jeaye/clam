#pragma once

#include "shared/network/socket.h"

#include <memory>

namespace net = shared::network;

namespace server
{
  class worker
  {
    public:
      worker() = delete;
      worker(net::address const &a, std::shared_ptr<net::socket> const s)
        : m_address(a), m_socket(s)
      { }
      worker(worker const&) = delete;
      worker(worker &&) = default;
      worker& operator =(worker const&) = delete;
      worker& operator =(worker &&) = default;

      net::address const& get_address() const
      { return m_address; }
      std::shared_ptr<net::socket> get_socket() const
      { return m_socket; }

    private:
      net::address m_address;
      std::shared_ptr<net::socket> m_socket;
  };

  struct worker_added
  {
    net::address const a;
    std::weak_ptr<worker> const w;
  };
}

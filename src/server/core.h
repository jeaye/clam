/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: core.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "shared/network/socket.h"
#include "generic_pool.h"
#include "worker.h"

#include "op/reader.h"
#include "op/pinger.h"
#include "op/stat_collector.h"

#include "shared/term/context.h"
#include "shared/term/window.h"

#include "ui/home.h"

#include <map>
#include <thread>
#include <chrono>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace net = shared::network;

namespace server
{
  class core
  {
    public:
      using worker_map = std::map<net::address, std::shared_ptr<worker>>;

      core();

      void run();
      void render();

      worker_map const& get_workers() const;

    private:
      void added_worker(net::socket::accept_result const &res);
      void accept();
      void render_impl();

      /* Our connection. */
      static net::port_t constexpr m_port{ 2272 };
      net::socket m_listener;
      bool m_running{ true };

      /* Only the core should own the workers -- XXX: everyone else use weak_ptr. */
      worker_map m_workers;

      /* Operators. */
      op::reader m_reader;
      op::pinger m_pinger{ std::chrono::milliseconds(1000) };
      op::stat_collector m_stat_collector;

      /* UI */
      shared::term::context m_context;
      shared::term::window m_root_window;
      ui::home m_home_window;
      std::mutex m_render_mutex;
      std::condition_variable m_render_condition;
      bool m_should_render{ false };
  };
}

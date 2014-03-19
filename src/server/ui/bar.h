/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: bar.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "shared/network/ip.h"
#include "shared/term/window.h"
#include "server/worker.h"

#include <string>
#include <future>
#include <chrono>

namespace server
{
  class core;

  namespace ui
  {
    class bar
    {
      public:
        bar() = delete;
        bar(core &c);

        void render();
        void render_worker(std::shared_ptr<worker> const w);

      private:
        void resize(shared::term::resize_event const &ev);

        shared::term::window m_host_window;
        shared::term::window m_stats_window;
        shared::term::window m_ip_window;
        shared::term::window m_cpu_window;
        shared::term::window m_worker_cpu_window;
        std::string m_internal_ip{ net::ip::get_internal() };

        /* External IP needs a web request, so it's set asynchronously. */
        std::string m_external_ip{ "Calculating" };
        std::future<std::string> m_external_ip_future;
        core &m_core;
    };
  }
}

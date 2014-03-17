/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: home.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include <future>
#include <chrono>

#include "shared/term/context.h"
#include "shared/term/window.h"
#include "shared/network/ip.h"

namespace net = shared::network;

namespace server
{
  namespace ui
  {
    class home
    {
      public:
        home();
        home(home const&) = delete;
        home(home &&) = delete;
        home& operator =(home const&) = delete;
        home& operator =(home &&) = delete;

        void render();

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
    };
  }
}

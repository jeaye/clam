/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: home.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

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
        home()
          : m_internal_ip(net::ip::get_internal())
          , m_external_ip(net::ip::get_external()) /* TODO: Get in async manner */
        {
          shared::term::context::pool_t::global().subscribe<shared::term::resize_event>(
              std::bind(&home::resize, this, std::placeholders::_1));
        }

        void render()
        {
          m_ip_window.render();
          /* TODO: Render port? */
          m_ip_window.render(0, 0, "internal: " + m_internal_ip);
          m_ip_window.render(0, 1, "external: " + m_external_ip);
          m_cpu_window.render();
        }

      private:
        void resize(shared::term::resize_event const &ev)
        {
          auto const bar_width(ev.width / 4);

          m_ip_window.set_x(ev.width - bar_width);
          m_ip_window.set_y((ev.height / 2) - 3);
          m_ip_window.set_dimensions(bar_width - 1, 3);

          m_cpu_window.set_x(ev.width - bar_width);
          m_cpu_window.set_y(ev.height / 2);
          m_cpu_window.set_dimensions(bar_width - 1, ev.height / 2);
        }

        shared::term::window m_host_window;
        shared::term::window m_stats_window;
        shared::term::window m_ip_window;
        shared::term::window m_cpu_window;
        std::string m_internal_ip;
        std::string m_external_ip;
    };
  }
}

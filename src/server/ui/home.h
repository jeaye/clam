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

namespace server
{
  namespace ui
  {
    class home
    {
      public:
        home()
        {
          shared::term::context::pool_t::global().subscribe<shared::term::resize_event>(
              std::bind(&home::resize, this, std::placeholders::_1));
        }

        void render()
        {
          m_stats_window.render();
        }

      private:
        void resize(shared::term::resize_event const &ev)
        {
          m_stats_window.set_x(ev.width - (ev.width / 4));
          m_stats_window.set_y(ev.height / 2);
          m_stats_window.set_dimensions((ev.width / 4) - 1, ev.height / 2);
        }

        shared::term::window m_host_window;
        shared::term::window m_stats_window;
        shared::term::window m_cpu_window;
        shared::term::window m_ip_window;
    };
  }
}

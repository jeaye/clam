/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: home.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "home.h"
#include "server/stat/cpu.h"

namespace server
{
  namespace ui
  {
    home::home()
    {
      shared::term::context::pool_t::global().subscribe<shared::term::resize_event>(
          std::bind(&home::resize, this, std::placeholders::_1));

      m_external_ip_future = std::async(std::launch::async, &net::ip::get_external);
    }

    void home::render()
    {
      if(m_external_ip_future.valid())
      {
        std::future_status status{ m_external_ip_future.wait_for(std::chrono::milliseconds(0)) };
        if(status == std::future_status::ready)
        { m_external_ip = m_external_ip_future.get(); }
      }

      m_ip_window.render();
      /* TODO: Render port? */
      m_ip_window.render(0, 0, "Internal: " + m_internal_ip);
      m_ip_window.render(0, 1, "External: " + m_external_ip);
      m_cpu_window.render();
      m_cpu_window.render(0, 0, "CPU Usage:");
      m_cpu_window.render(0, 1, stat::cpu_bar(m_cpu_window.get_width() - 2));
    }

    void home::resize(shared::term::resize_event const &ev)
    {
      size_t constexpr const bar_max_width{ 26 };
      auto const bar_width(std::min<size_t>(ev.width / 4, bar_max_width));

      m_ip_window.set_x(ev.width - bar_width);
      m_ip_window.set_y((ev.height / 2) - 3);
      m_ip_window.set_dimensions(bar_width - 1, 3);

      m_cpu_window.set_x(ev.width - bar_width);
      m_cpu_window.set_y(ev.height / 2);
      m_cpu_window.set_dimensions(bar_width - 1, ev.height / 2);
    }
  }
}

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

      m_ip_window.set_title("IP");
      m_cpu_window.set_title("Stats");
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

      /* TODO: Cache? Don't actually check these each frame. */
      m_cpu_window.render();
      m_cpu_window.render(0, 0, "CPU Usage:");
      m_cpu_window.render(0, 1, stat::cpu_bar(m_cpu_window.get_width() - 2));
      m_cpu_window.render(0, 2, "RAM Usage:");
      m_cpu_window.render(0, 3, stat::ram_bar(m_cpu_window.get_width() - 2));
    }

    void home::resize(shared::term::resize_event const &ev)
    {
      size_t constexpr const bar_max_width{ 26 };
      auto const bar_width(std::min<size_t>(ev.width / 4, bar_max_width));

      size_t constexpr const spacing{ 1 };
      size_t constexpr const cpu_height{ 5 };
      size_t constexpr const ip_height{ 3 };

      m_ip_window.set_x(ev.width - bar_width);
      m_ip_window.set_y(ev.height - cpu_height - ip_height - spacing * 2);
      m_ip_window.set_dimensions(bar_width - 1, ip_height);

      m_cpu_window.set_x(ev.width - bar_width);
      m_cpu_window.set_y(ev.height - cpu_height - spacing);
      m_cpu_window.set_dimensions(bar_width - 1, cpu_height);
    }
  }
}

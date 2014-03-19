/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: bar.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "bar.h"
#include "shared/stat/cpu.h"
#include "server/core.h"

namespace server
{
  namespace ui
  {
    bar::bar(core &c)
      : m_core(c)
    {
      shared::term::context::pool_t::global().subscribe<shared::term::resize_event>(
          std::bind(&bar::resize, this, std::placeholders::_1));

      m_external_ip_future = std::async(std::launch::async, &net::ip::get_external);

      m_ip_window.set_title("IP");
      m_cpu_window.set_title("System Stats");
      m_worker_cpu_window.set_title("Worker Stats");
    }

    void bar::render()
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
      m_cpu_window.render(0, 1, shared::stat::cpu_bar(m_cpu_window.get_width() - 2));
      m_cpu_window.render(0, 2, "RAM Usage:");
      m_cpu_window.render(0, 3, shared::stat::ram_bar(m_cpu_window.get_width() - 2));

    }

    void bar::render_worker(std::shared_ptr<worker> const w)
    {
      auto const stats(m_core.worker_stat(w->get_address()));
      m_worker_cpu_window.render();
      m_worker_cpu_window.render(0, 0, "CPU Usage:");
      m_worker_cpu_window.render(0, 1, shared::stat::make_bar(m_cpu_window.get_width() - 2, stats.cpu));
      m_worker_cpu_window.render(0, 2, "RAM Usage:");
      m_worker_cpu_window.render(0, 3, shared::stat::make_bar(m_cpu_window.get_width() - 2, stats.ram));
    }

    void bar::resize(shared::term::resize_event const &ev)
    {
      size_t constexpr const bar_max_width{ 26 };
      auto const bar_width(std::min<size_t>(ev.width / 4, bar_max_width));

      size_t constexpr const spacing{ 1 };
      size_t constexpr const cpu_height{ 5 };
      size_t constexpr const ip_height{ 3 };

      m_worker_cpu_window.set_x(ev.width - bar_width);
      m_worker_cpu_window.set_y(ev.height - cpu_height - ip_height - cpu_height - spacing * 3);
      m_worker_cpu_window.set_dimensions(bar_width - 1, cpu_height);

      m_ip_window.set_x(ev.width - bar_width);
      m_ip_window.set_y(ev.height - cpu_height - ip_height - spacing * 2);
      m_ip_window.set_dimensions(bar_width - 1, ip_height);

      m_cpu_window.set_x(ev.width - bar_width);
      m_cpu_window.set_y(ev.height - cpu_height - spacing);
      m_cpu_window.set_dimensions(bar_width - 1, cpu_height);
    }
  }
}

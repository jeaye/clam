/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: core.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "core.h"
#include "log/logger.h"

namespace server
{
  core::core()
  {
    proto::initialize();
    m_listener.listen(m_port);

    generic_pool_t::global().subscribe<net::socket::accept_result>(
        std::bind(&core::added_worker, this, std::placeholders::_1));

    m_context.set_clear_color(shared::term::color_attrib::green,
                              shared::term::color_attrib::black);

    m_root_window.set_pos(1, 1);
    m_root_window.set_dimensions(m_context.get_width() - 2,
                                 m_context.get_height() - 2);

    shared::term::context::pool_t::global().subscribe<shared::term::key_event>(
    [&](shared::term::key_event const &ev)
    {
      if(ev.k == shared::term::key::escape)
      { throw std::runtime_error("escape pressed"); }
    });
    shared::term::context::pool_t::global().subscribe<shared::term::resize_event>(
    [&](shared::term::resize_event const &ev)
    {
      m_root_window.set_width(ev.width - 2);
      m_root_window.set_height(ev.height - 2);
    });

    /* Send an initial resize event. */
    shared::term::context::pool_t::global().post<shared::term::resize_event>(
        { m_context.get_width(), m_context.get_height(), 0, 0 });
  }

  void core::run()
  {
    std::thread accept_thread(std::bind(&core::accept, this));
    std::thread render_thread(std::bind(&core::render_impl, this));
    try
    {
      while(m_running)
      {
        while(generic_pool_t::global().poll()) ;

        m_reader(m_workers);
        m_pinger(m_workers);
        m_stat_collector(m_workers);

        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    }
    catch(std::exception const &e)
    {
      log_system("core exception: %%", e.what());
      m_running = false;

      /* Wake up the render thread. */
      render();
      accept_thread.detach();
      render_thread.detach();
    }
  }

  void core::render()
  {
    /* Notify the render thread that it's time to work. */
    {
      std::lock_guard<std::mutex> const lock{ m_render_mutex };
      m_should_render = true;
    }
    m_render_condition.notify_one();
  }

  core::worker_map const& core::get_workers() const
  { return m_workers; }

  op::stat_collector::entry core::worker_stat(net::address const &a)
  { return m_stat_collector[a]; }

  void core::added_worker(net::socket::accept_result const &res)
  {
    auto const shared(std::make_shared<worker>(res.sender, res.sock));
    m_workers.emplace(std::make_pair(res.sender, shared));

    generic_pool_t::global().post(worker_added{ res.sender, shared, });
  }

  void core::accept()
  {
    /* Dedicated thread for accepting connections. */
    try
    {
      while(m_running)
      {
        auto res(m_listener.accept());
        log_worker(res.sender, "accepted connection");
        generic_pool_t::global().post(res);
      }
    }
    catch(std::exception const &e)
    {
      m_running = false;
      log_system("acception: %%", e.what());
    }
  }

  void core::render_impl()
  {
    /* Dedicated thread for rendering. */
    try
    {
      while(m_running)
      {
        /* Wait to be told when to render. */
        std::unique_lock<std::mutex> lock{ m_render_mutex };
        m_render_condition.wait(lock, [&]{ return m_should_render; });

        /* We may've been woken up to die. */
        if(!m_running)
        { break; }

        m_context.clear();
        m_root_window.render();

        /* XXX: Read-only access while other threads may be *adding* lines only. */
        /* TODO: Render in separate window. */
        for(size_t i{}; i < logging::chrono_buffer.size(); ++i)
        { m_root_window.render(0, i, logging::chrono_buffer[i].get()); }

        m_home_window.render();
        m_context.present();
        m_context.poll();

        m_should_render = false;
      }
    }
    catch(std::exception const &e)
    {
      m_running = false;
      log_system("render exception: %%" ,e.what());
    }
  }
}

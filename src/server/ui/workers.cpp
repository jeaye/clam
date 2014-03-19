/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: workers.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "workers.h"
#include "server/core.h"

namespace server
{
  namespace ui
  {
    workers::workers(core &c)
      : m_core(c)
      , m_bar(c)
    {
      shared::term::context::pool_t::global().subscribe<shared::term::resize_event>(
          std::bind(&workers::resize, this, std::placeholders::_1));

      m_list_window.set_pos(1, 1);
      m_list_window.set_dimensions(shared::term::context::global().get_width() - 2,
                                   shared::term::context::global().get_height() - 2);
    }

    void workers::render()
    {
      auto const &workers(m_core.get_workers());

      m_list_window.render();
      std::stringstream ss;
      size_t i{};
      for(auto it(workers.begin()); it != workers.end(); ++i, ++it, ss.str(""))
      {
        ss << "#" << i << ": " << it->first;
        m_list_window.render(0, i, ss.str());
      }

      m_bar.render();
      if(workers.size())
      {
        /* TODO: Check the *selected* worker, not just the first. */
        auto const beg(workers.begin());
        m_bar.render_worker(beg->second);
      }
    }

    void workers::resize(shared::term::resize_event const &ev)
    { m_list_window.set_dimensions(ev.width - 2, ev.height - 2); }
  }
}


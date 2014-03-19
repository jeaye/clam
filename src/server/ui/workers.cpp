/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: workers.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "workers.h"
#include "server/core.h"

#include <iterator>

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
      shared::term::context::pool_t::global().subscribe<shared::term::key_event>(
      [&](shared::term::key_event const &ev)
      {
        auto const &workers(m_core.get_workers());
        if(ev.ch == U'j')
        {
          if(++m_selection == static_cast<int32_t>(workers.size()))
          { m_selection = -1; }
        }
        else if(ev.ch == U'k')
        {
          if(--m_selection == -2)
          { m_selection = workers.size() - 1; }
        }
        if(m_selection >= 0)
        {
          auto beg(workers.begin());
          std::advance(beg, m_selection);
          m_selected_worker = beg->second;
        }
        else
        { m_selected_worker.reset(); }
      });

      m_list_window.set_pos(1, 1);
      m_list_window.set_dimensions(shared::term::context::global().get_width() - 2,
                                   shared::term::context::global().get_height() - 2);
    }

    void workers::render()
    {
      auto const &workers(m_core.get_workers());

      m_list_window.render();
      std::stringstream ss;
      int32_t i{};
      for(auto it(workers.begin()); it != workers.end(); ++i, ++it, ss.str(""))
      {
        if(i == m_selection)
        { ss << "> "; }
        else
        { ss << "  "; }

        ss << "#" << i << ": " << it->first;
        m_list_window.render(0, i, ss.str());
      }

      m_bar.render();
      auto shared(m_selected_worker.lock());
      if(shared)
      { m_bar.render_worker(shared); }
    }

    void workers::resize(shared::term::resize_event const &ev)
    {
      m_list_window.set_dimensions(ev.width - 2 - m_bar.get_width(),
                                   ev.height - 2);
    }
  }
}


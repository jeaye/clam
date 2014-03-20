/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: home.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "home.h"
#include "server/core.h"

namespace server
{
  namespace ui
  {
    home::home(core &c)
      : m_core(c)
      , m_bar(c)
    {
      shared::term::context::pool_t::global().subscribe<shared::term::resize_event>(
          std::bind(&home::resize, this, std::placeholders::_1));
    }

    void home::render()
    {
      m_bar.render();
    }

    void home::resize(shared::term::resize_event const &)
    { }
  }
}

/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: home.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "bar.h"

namespace net = shared::network;

namespace server
{
  class core;

  namespace ui
  {
    class home
    {
      public:
        home() = delete;
        home(core &c);
        home(home const&) = delete;
        home(home &&) = delete;
        home& operator =(home const&) = delete;
        home& operator =(home &&) = delete;

        void render();

      private:
        void resize(shared::term::resize_event const &ev);

        core &m_core;
        bar m_bar;
    };
  }
}

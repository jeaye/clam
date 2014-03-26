/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: workers.h
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
    class workers
    {
      public:
        workers() = delete;
        workers(core &c);
        workers(workers const&) = delete;
        workers(workers &&) = delete;
        workers& operator =(workers const&) = delete;
        workers& operator =(workers &&) = delete;

        void render();

      private:
        void resize_event(shared::term::resize_event const &ev);
        void key_event(shared::term::key_event const &ev);

        core &m_core;
        bar m_bar;
        shared::term::window m_list_window;
        shared::term::window m_log_window;

        /* UI selection */
        int32_t m_selection{ -1 };
        std::weak_ptr<worker> m_selected_worker;
    };
  }
}

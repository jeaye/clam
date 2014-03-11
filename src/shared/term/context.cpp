/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: context.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "context.h"

namespace shared
{
  namespace term
  {
    static context *glob{};

    context::context()
    {
      if(glob)
      { throw std::runtime_error("There may only be one context"); }
      glob = this;

      auto const ret(tb_init());
      if(ret < 0)
      { throw std::runtime_error("Failed to initialize terminal"); }

      set_output_mode(output_mode::_256);

      m_width = tb_width();
      m_height = tb_height();
    }

    context& context::global()
    {
      if(!glob)
      { throw std::runtime_error("No current context"); }
      else
      { return *glob; }
    }

    void context::poll()
    {
      tb_event ev;
      while(true)
      {
        auto const ret(tb_peek_event(&ev, 0));

        if(ret < 0) /* Error */
        { throw std::runtime_error("Error while polling events"); }
        else if(ret == 0) /* No event */
        { break; }
        else
        { handle_event(ev); }
      }

      while(pool_t::global().poll());
    }

    void context::wait_poll()
    {
      tb_event ev;
      auto const ret(tb_poll_event(&ev));

      if(ret < 0) /* Error */
      { throw std::runtime_error("Error while polling events"); }
      else
      { handle_event(ev); }

      while(pool_t::global().poll());
    }

    void context::handle_event(tb_event &ev)
    {
      switch(ev.type)
      {
        case TB_EVENT_KEY:
        {
          /* Treat space as a char. */
          if(ev.key == static_cast<int>(key::space))
          { ev.ch = U' '; }

          pool_t::global().post<key_event>(
          { static_cast<key>(ev.key), ev.ch, static_cast<modifier>(ev.mod) });
        } break;

        case TB_EVENT_RESIZE:
        {
          auto const width(m_width);
          auto const height(m_height);
          m_width = ev.w;
          m_height = ev.h;

          pool_t::global().post<resize_event>(
              { ev.w, ev.h, (ev.w - width), (ev.h - height) });
        } break;

        default:
          throw std::runtime_error("Unexpected event type");
      }
    }

    void context::set_output_mode(output_mode const o)
    {
      m_output_mode = o;
      tb_select_output_mode(static_cast<int>(o));
    }
  }
}

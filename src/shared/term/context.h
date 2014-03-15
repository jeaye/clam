/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: context.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include <stdexcept>

#include "termbox.h"

#include "cell_string.h"
#include "shared/notif/pool.h"
#include "event.h"

namespace shared
{
  namespace term
  {
    enum class output_mode
    {
      /* Black, red, green, yellow, blue, magenta, cyan, white. */
      _8 = TB_OUTPUT_NORMAL, 
      _256 = TB_OUTPUT_256,
      _216 = TB_OUTPUT_216,
      _24 = TB_OUTPUT_GRAYSCALE
    };

    /* TODO: escape mode? */
    class context
    {
      public:
        using pool_t = notif::pool<context>;

        context();
        context(context const&) = delete;
        context(context &&) = delete;
        ~context()
        { tb_shutdown(); }
        context& operator =(context const&) = delete;
        context& operator =(context &&) = delete;

        static context& global();

        pos_t get_width() const
        { return m_width; }
        pos_t get_height() const
        { return m_height; }

        void set_cursor(pos_t const x, pos_t const y)
        {
          m_cursor_x = x;
          m_cursor_y = y;
          if(m_show_cursor)
          { tb_set_cursor(m_cursor_x, m_cursor_y); }
        }
        void hide_cursor()
        {
          m_show_cursor = false;
          tb_set_cursor(TB_HIDE_CURSOR, TB_HIDE_CURSOR);
        }
        void show_cursor()
        {
          m_show_cursor = true;
          tb_set_cursor(m_cursor_x, m_cursor_y);
        }
        pos_t get_cursor_x() const
        { return m_cursor_x; }
        pos_t get_cursor_y() const
        { return m_cursor_y; }

        void clear()
        { tb_clear(); }
        void set_clear_color(color_attrib const fg, color_attrib const bg)
        { tb_set_clear_attributes(m_clear_fg = static_cast<attrib_t>(fg) - 1,
                                  m_clear_bg = static_cast<attrib_t>(bg) - 1); }
        void set_clear_color(attrib_t const fg, attrib_t const bg)
        { tb_set_clear_attributes(m_clear_fg = fg, m_clear_bg = bg); }
        attrib_t get_fg() const
        { return m_clear_fg; }
        attrib_t get_bg() const
        { return m_clear_bg; }

        void present()
        { tb_present(); }
        void poll();
        void wait_poll();

      private:
        void handle_event(tb_event &ev);
        void set_output_mode(output_mode const o);
        output_mode get_output_mode() const
        { return static_cast<output_mode>(tb_select_output_mode(TB_OUTPUT_CURRENT)); }

        output_mode m_output_mode{ output_mode::_256 };
        attrib_t m_clear_fg{}, m_clear_bg{};
        pos_t m_width{}, m_height{};
        pos_t m_cursor_x{}, m_cursor_y{};
        bool m_show_cursor{ false };
    };
  }
}

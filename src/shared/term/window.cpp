/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: window.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "window.h"

namespace shared
{
  namespace term
  {
    window::window()
    {
      m_attrib_state.fg = static_cast<attrib_t>(color_attrib::red);
      m_attrib_state.bg = static_cast<attrib_t>(color_attrib::black);
      set_width(2);
      set_height(2);
    }

    void window::set_x(pos_t const x)
    {
      m_x = x;
      m_inside_x = m_x + 1;
    }

    void window::set_y(pos_t const y)
    {
      m_y = y;
      m_inside_y = m_y + 1;
    }

    void window::set_pos(pos_t const x, pos_t const y)
    { set_x(x); set_y(y); }

    void window::set_width(pos_t const w)
    { m_width = w; }

    void window::set_height(pos_t const h)
    { m_height = h; }

    void window::set_dimensions(pos_t const w, pos_t const h)
    { set_width(w); set_height(h); }

    void window::set_cursor(pos_t const x, pos_t const y)
    { context::global().set_cursor(x, y); }

    void window::increment_cursor()
    {
      auto x(context::global().get_cursor_x());
      auto y(context::global().get_cursor_y());
      if(x == context::global().get_width())
      {
        x = 0;
        if(y < context::global().get_height())
        { ++y; }
      }
      else
      { ++x; }
      context::global().set_cursor(x, y);
    }

    void window::set_fg(attrib_t const fg)
    { m_attrib_state.fg = fg; }

    void window::set_bg(attrib_t const bg)
    { m_attrib_state.bg = bg; }

    void window::set_title(std::string const &t)
    { m_title = t; }

    void window::render()
    {
      for(pos_t x{}; x < m_width - 1; ++x)
      {
        for(pos_t y{}; y < m_height - 1; ++y)
        { render(x, y, " "); }
      }

      /* Horizontal */
      for(pos_t i{ -1 }; i < m_width; ++i)
      {
        m_borders.t.x = i;
        m_borders.t.y = -1;
        render(m_borders.t);

        m_borders.b.x = i;
        m_borders.b.y = m_height - 1;
        render(m_borders.b);
      }
      /* Vertical */
      for(pos_t i{ -1 }; i < m_height; ++i)
      {
        m_borders.l.x = -1;
        m_borders.l.y = i;
        render(m_borders.l);

        m_borders.r.x = m_width - 1;
        m_borders.r.y = i;
        render(m_borders.r);
      }
      /* Corners */
      m_borders.tl.x = m_borders.tl.y = -1;
      render(m_borders.tl);

      m_borders.tr.x = m_width - 1;
      m_borders.tr.y = -1;
      render(m_borders.tr);

      m_borders.br.x = m_width - 1;
      m_borders.br.y = m_height - 1;
      render(m_borders.br);

      m_borders.bl.x = -1;
      m_borders.bl.y = m_height - 1;
      render(m_borders.bl);

      /* Title */
      if(m_title.size())
      {
        text t;
        t.data = m_title;
        t.sty.align = alignment::center;
        t.sty.color = coloring::inherit_fg;
        t.y = -1;
        render(t);
      }
    }

    void window::render(pos_t const ox, pos_t const oy, char const * const str,
        style const s)
    { render(ox, oy, std::string{ str }, s); }

    /* Single unicode char, no coloring. */
    void window::render(pos_t const x, pos_t const y, unicode_t const c)
    {
      set_cursor(x, y);
      tb_change_cell(m_inside_x + x, m_inside_y + y, c,
                     m_attrib_state.fg - 1, m_attrib_state.bg - 1);
      increment_cursor();
    }

    /* Pre-built cells, possible coloring. */
    void window::render(pos_t const x, pos_t const y, cell const &c)
    { 
      attrib_t fg{}, bg{};
      if(c.fg.color != color_attrib::none)
      { fg |= static_cast<int>(c.fg.color) - 1; }
      else
      { fg |= m_attrib_state.fg - 1; }
      if(c.bg.color != color_attrib::none)
      { bg |= static_cast<int>(c.bg.color) - 1; }
      else
      { bg |= m_attrib_state.bg - 1; }
      fg |= static_cast<int>(c.fg.attr);
      bg |= static_cast<int>(c.bg.attr);

      set_cursor(x, y);
      tb_change_cell(m_inside_x + x, m_inside_y + y, c.ch, fg, bg);
      increment_cursor();
    }

    void window::render(pos_t x, pos_t y, cell_string const &str)
    {
      for(auto const &c : str)
      { render(x++, y, c); }
    }

    void window::render_cell(pos_t const x, pos_t const y, char const ch,
        attrib_t const fg, attrib_t const bg)
    {
      set_cursor(x, y);
      unicode_t unch{};
      tb_utf8_char_to_unicode(&unch, &ch);
      tb_change_cell(x, y, unch, fg, bg);
      increment_cursor();
    }

    void window::render_cell(pos_t const x, pos_t const y, cell const c,
        attrib_t const fg, attrib_t const bg)
    {
      set_cursor(x, y);
      tb_change_cell(x, y, c.ch, fg, bg);
      increment_cursor();
    }

    void window::render_cell(pos_t const x, pos_t const y, unicode_t const ch,
        attrib_t const fg, attrib_t const bg)
    {
      set_cursor(x, y);
      tb_change_cell(x, y, ch, fg, bg);
      increment_cursor();
    }
  }
}

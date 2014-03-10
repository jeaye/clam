/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: window.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "context.h"
#include "text.h"

namespace shared
{
  namespace term
  {
    struct attrib_state
    { attrib_t fg{}, bg{}; };

    struct border_text : public unicode_text
    {
      border_text(unicode_t const ch)
      {
        data.push_back(ch);
        sty = { wrapping::none, alignment::left, coloring::none };
      }
    };
    struct border_cells
    {
      border_text tl{ U'┌' }, t{ U'─' }, tr{ U'┐' },
                   l{ U'│' },             r{ U'│' },
                  bl{ U'└' }, b{ U'─' }, br{ U'┘' };
    };

    class window
    {
      public:
        window();

        void set_x(pos_t const x);
        void set_y(pos_t const y);
        void set_pos(pos_t const x, pos_t const y);
        
        void set_width(pos_t const w);
        void set_height(pos_t const h);
        void set_dimensions(pos_t const w, pos_t const h);

        void set_fg(attrib_t const fg);
        void set_bg(attrib_t const bg);

        void draw();

        void draw(pos_t const ox, pos_t const oy, char const * const str,
                  style const s = style{});

        template <typename String>
        void draw(pos_t const ox, pos_t const oy, String const &str,
                  style const s = style{})
        {
          pos_t x{ ox }, y{ oy };
          attrib_t fg(m_attrib_state.fg - 1), bg(m_attrib_state.bg - 1);
          switch(s.color)
          {
            case coloring::inherit_both:
            { } break;
            case coloring::inherit_fg:
            {
              bg = context::global().get_bg();
            } break;
            case coloring::inherit_bg:
            {
              fg = context::global().get_fg();
            } break;
            case coloring::none:
            {
              fg = context::global().get_fg();
              bg = context::global().get_bg();
            } break;
            default:
              throw std::runtime_error("Bad coloring mode");
          }
          
          switch(s.wrap)
          {
            case wrapping::none:
            {
              for(auto const ch : str)
              {
                if(ch == U'\n')
                { x = 0; ++y; set_cursor(m_inside_x + x, m_inside_y + y); }
                else if(ch == U'\r')
                { x = 0; set_cursor(m_inside_x + x, m_inside_y + y); }
                else
                { draw_cell(m_inside_x + x++, m_inside_y + y, ch, fg, bg); }
                if(x >= (m_x + m_width - 2) || y >= (m_y + m_height - 2))
                { break; }
              }
            } break;
            case wrapping::character:
            {
              for(auto const ch : str)
              {
                if(ch == U'\n')
                { x = 0; ++y; set_cursor(m_inside_x + x, m_inside_y + y); }
                else if(ch == U'\r')
                { x = 0; set_cursor(m_inside_x + x, m_inside_y + y); }
                else
                { draw_cell(m_inside_x + x++, m_inside_y + y, ch, fg, bg); }
                if(x > m_width - 2)
                { x = 0; ++y; }
                if(y >= (m_y + m_height - 2))
                { break; }
              }
            } break;
            case wrapping::word:
            {
              for(size_t i{}; i < str.size(); ++i)
              {
                if(str[i] == U'\n')
                { x = 0; ++y; set_cursor(m_inside_x + x, m_inside_y + y); }
                else if(str[i] == U'\r')
                { x = 0; set_cursor(m_inside_x + x, m_inside_y + y); }
                else
                { draw_cell(m_inside_x + x++, m_inside_y + y, str[i], fg, bg); }
                if(x > m_width - 2)
                {
                  /* If it's whitespace, we're cool. */
                  if(str[i] == ' ' || str[i] == '-')
                  { x = 0; ++y; }
                  else
                  {
                    while(str[i] != ' ')
                    {
                      --i; --x;
                      draw(x, y, " ");

                      /* The word is too long... switch to char wrapping. */
                      if(x == 0 || i == 0) 
                      {
                        auto sty(s);
                        sty.wrap = wrapping::character;
                        draw(ox, oy, str, sty); /* recurse */
                        return;
                      }
                    }
                    x = 0; ++y;
                  }
                }
                if(y >= (m_y + m_height - 2))
                { break; }
              }
            } break;
          }
        }
        
        /* Single unicode char, no coloring. */
        void draw(pos_t const x, pos_t const y, unicode_t const c);

        /* Pre-built cells, possible coloring. */
        void draw(pos_t const x, pos_t const y, cell const &c);
        void draw(pos_t x, pos_t y, cell_string const &str);

        template <typename Text>
        void draw(Text const &t)
        {
          switch(t.sty.align)
          {
            case alignment::left:
            {
              draw(t.x, t.y, t.data, t.sty);
            } break;
            case alignment::right:
            {
              draw(m_width - 1 - t.data.size(), t.y, t.data, t.sty);
            } break;
            case alignment::center:
            {
              draw((m_width >> 1) - (t.data.size() >> 1), t.y, t.data, t.sty);
            } break;
            default:
              throw std::runtime_error("Invalid text alignment: " +
                                       std::to_string(static_cast<int>(t.sty.align)));
          }
        }

      private:
        void draw_cell(pos_t const x, pos_t const y, char const ch,
                       attrib_t const fg, attrib_t const bg);
        void draw_cell(pos_t const x, pos_t const y, cell const c,
                       attrib_t const fg, attrib_t const bg);
        void draw_cell(pos_t const x, pos_t const y, unicode_t const ch,
                       attrib_t const fg, attrib_t const bg);
        void set_cursor(pos_t const x, pos_t const y);
        void increment_cursor();

        pos_t m_x{}, m_y{};
        pos_t m_inside_x{}, m_inside_y{};
        pos_t m_width{}, m_height{};
        attrib_state m_attrib_state{};
        border_cells m_borders;
        context &m_context{ context::global() };
    };
  }
}

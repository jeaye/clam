/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: cell.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include <string>

#include "termbox.h"

namespace shared
{
  namespace term
  {
    using attrib_t = uint16_t;
    using pos_t = int;
    using unicode_t = uint32_t;

    enum class fg_attrib
    {
      none,
      bold = TB_BOLD,
      underline = TB_UNDERLINE,
      reverse = TB_REVERSE
    };
    enum class bg_attrib
    {
      none,
      bold = TB_BOLD,
      underline = TB_UNDERLINE,
      reverse = TB_REVERSE
    };
    enum class color_attrib
    {
      none,

      black,
      red,
      green,
      yellow,
      blue,
      magenta,
      cyan,
      white,

      bright_black,
      bright_red,
      bright_green,
      bright_yellow,
      bright_blue,
      bright_magenta,
      bright_cyan,
      bright_white,
    };

    struct cell
    {
      cell() = default;
      cell(char const c)
        : ch(), fg(), bg()
      { tb_utf8_char_to_unicode(&ch, &c); }
      cell(unicode_t const c)
        : ch(c), fg(), bg()
      { }

      unicode_t ch;
      struct fg_
      {
        color_attrib color;
        fg_attrib attr;
      } fg;
      struct bg_
      {
        color_attrib color;
        bg_attrib attr;
      } bg;
    };

    inline fg_attrib operator | (fg_attrib const lhs, fg_attrib const rhs)
    { return static_cast<fg_attrib>(static_cast<int>(lhs) |
                                    static_cast<int>(rhs)); }
    inline fg_attrib& operator |= (fg_attrib &lhs, fg_attrib const rhs)
    { return lhs = static_cast<fg_attrib>(static_cast<int>(lhs) |
                                          static_cast<int>(rhs)); }

    inline cell operator | (cell &c, fg_attrib const a)
    {
      cell r(c);
      r.fg.attr |= a;
      return r;
    }
    inline cell& operator |= (cell &c, fg_attrib const a)
    {
      c.fg.attr |= a;
      return c;
    }
    inline bool operator ==(attrib_t const i, fg_attrib const a)
    { return i == static_cast<attrib_t>(a); }
    inline bool operator ==(fg_attrib const a, attrib_t const i)
    { return i == static_cast<attrib_t>(a); }

    inline bg_attrib operator | (bg_attrib const lhs, bg_attrib const rhs)
    { return static_cast<bg_attrib>(static_cast<int>(lhs) |
                                    static_cast<int>(rhs)); }
    inline bg_attrib& operator |= (bg_attrib &lhs, bg_attrib const rhs)
    { return lhs = static_cast<bg_attrib>(static_cast<int>(lhs) |
                                          static_cast<int>(rhs)); }

    inline cell operator | (cell &c, bg_attrib const a)
    {
      cell r(c);
      r.bg.attr |= a;
      return r;
    }
    inline cell& operator |= (cell &c, bg_attrib const a)
    {
      c.bg.attr |= a;
      return c;
    }
    inline bool operator ==(int const i, bg_attrib const a)
    { return i == static_cast<int>(a); }
    inline bool operator ==(bg_attrib const a, int const i)
    { return i == static_cast<int>(a); }

    inline bool operator ==(int const i, color_attrib const a)
    { return i == static_cast<int>(a); }
    inline bool operator ==(color_attrib const a, int const i)
    { return i == static_cast<int>(a); }

    inline color_attrib operator | (color_attrib const lhs, fg_attrib const rhs)
    { return static_cast<color_attrib>(static_cast<int>(lhs) |
                                       static_cast<int>(rhs)); }
    inline color_attrib& operator |= (color_attrib &lhs, fg_attrib const rhs)
    { return lhs = static_cast<color_attrib>(static_cast<int>(lhs) |
                                             static_cast<int>(rhs)); }
    inline color_attrib operator | (color_attrib const lhs, bg_attrib const rhs)
    { return static_cast<color_attrib>(static_cast<int>(lhs) |
                                       static_cast<int>(rhs)); }
    inline color_attrib& operator |= (color_attrib &lhs, bg_attrib const rhs)
    { return lhs = static_cast<color_attrib>(static_cast<int>(lhs) |
                                             static_cast<int>(rhs)); }

    inline fg_attrib operator | (fg_attrib const lhs, color_attrib const rhs)
    { return static_cast<fg_attrib>(static_cast<int>(lhs) |
                                    static_cast<int>(rhs)); }
    inline fg_attrib& operator |= (fg_attrib &lhs, color_attrib const rhs)
    { return lhs = static_cast<fg_attrib>(static_cast<int>(lhs) |
                                          static_cast<int>(rhs)); }
    inline bg_attrib operator | (bg_attrib const lhs, color_attrib const rhs)
    { return static_cast<bg_attrib>(static_cast<int>(lhs) |
                                    static_cast<int>(rhs)); }
    inline bg_attrib& operator |= (bg_attrib &lhs, color_attrib const rhs)
    { return lhs = static_cast<bg_attrib>(static_cast<int>(lhs) |
                                          static_cast<int>(rhs)); }
  }
}

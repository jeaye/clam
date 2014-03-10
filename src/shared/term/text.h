/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: text.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include <string>

#include "cell.h"

namespace shared
{
  namespace term
  {
    enum class wrapping
    {
      /* Wraps at a word boundry. If a word is too
       * large for the window, character wrapping
       * will be used. */
      word,
      character,
      /* Truncates. */
      none
    };

    /* Horizontal alignment only. */
    enum class alignment
    {
      left,
      /* Ignores x if centered */
      center,
      right
    };

    enum class coloring
    {
      inherit_both,
      inherit_fg,
      inherit_bg,
      none
    };

    struct style
    {
      wrapping wrap;
      alignment align;
      coloring color;
    };

    template <typename String>
    struct text_impl
    {
      public:
        text_impl() = default;
        text_impl(std::string const &s)
          : data(s)
        { }

        String data;
        style sty{};
        pos_t x{}, y{};
    };
    using text = text_impl<std::string>;
    using cell_text = text_impl<cell_string>;
    using unicode_text = text_impl<std::basic_string<unicode_t>>;
  }
}

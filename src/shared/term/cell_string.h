/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: cell_string.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "cell.h"

namespace shared
{
  namespace term
  {
    class cell_string
    {
      public:
        using str_t = std::basic_string<cell>;
        using iterator = str_t::iterator;
        using const_iterator = str_t::const_iterator;
        using size_type = str_t::size_type;

        cell_string() = default;
        cell_string(std::string const &str)
        {
          m_str.resize(str.size());
          for(size_type i{}; i < str.size(); ++i)
          {
            unicode_t unch{};
            tb_utf8_char_to_unicode(&unch, &str[i]);
            m_str[i].ch = unch;
          }
        }

        iterator begin()
        { return m_str.begin(); }
        const_iterator begin() const
        { return m_str.begin(); }
        const_iterator cbegin() const
        { return m_str.cbegin(); }
        iterator end()
        { return m_str.end(); }
        const_iterator end() const
        { return m_str.end(); }
        const_iterator cend() const
        { return m_str.cend(); }

        size_type size() const
        { return m_str.size(); }

      private:
        str_t m_str;
    };
  }
}

/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: format.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include <string>
#include <stdexcept>
#include <ostream>

namespace server
{
  namespace logging
  {
    class variadic_format
    {
      public:
        variadic_format(std::ostream &os)
          : m_out(os)
        { m_out << std::boolalpha; }

        void print(char const * const format)
        {
          /* Check for remaining occurrences of %%. */
          for(char const *tmp{ format }; *tmp; ++tmp)
          {
            if(tmp && *tmp == '%' && *(tmp + 1) == '%')
            { throw std::out_of_range("Not enough format arguments supplied"); }
          }
          m_out << format;
        }

        template <typename T, typename... Args>
        void print(char const *format, T &&value, Args&&... args)
        {
          for(; *format; ++format)
          {
            if(*format == '%' && *(format + 1) == '%')
            {
              m_out << std::forward<T>(value);
              print(format + 2, std::forward<Args>(args)...);
              return;
            }
            m_out << *format;
          }
          /* Didn't find %%. */
          throw std::out_of_range("Too many format arguments");
        }

      private:
        std::ostream &m_out;
    };
  }
}

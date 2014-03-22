/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: resource.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include <functional>

namespace shared
{
  namespace util
  {
    template <typename T, typename D = std::function<void (T)>>
    class resource
    {
      public:
        resource() = default;
        resource(T const &t)
          : m_data(t)
        { }
        resource(T const &t, D const &d)
          : m_data(t)
          , m_deleter(d)
        { }
        ~resource()
        {
          if(m_deleter)
          { m_deleter(m_data); }
        }

        T& get()
        { return m_data; }
        T const& get() const
        { return m_data; }

      private:
        T m_data{};
        D m_deleter{};
    };
  }
}

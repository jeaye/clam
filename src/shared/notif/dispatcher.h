/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: dispatcher.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include <vector>
#include <functional>

namespace shared
{
  namespace notif
  {
    enum class state
    {
      subscribed,
      unsubscribed
    };

    template <typename T>
    class callback
    {
      public:
        /* Callbacks must return true to remain subscribed. */
        using func_t = std::function<state (T const&)>;
        using tag_t = uint64_t;

        callback() = delete;
        callback(func_t const &func, tag_t const tag)
          : m_func(func), m_tag(tag)
        { }
        callback(callback<T> &&) = default;
        callback(callback<T> const&) = default;
        callback<T>& operator =(callback<T> const &) = default;

        state operator ()(T const &data) const
        { return m_func(data); }

      private:
        func_t m_func;
        tag_t m_tag{};
    };

    template <typename Owner, typename T>
    class dispatcher
    {
      public:
        using val_t = callback<T>;
        using tag_t = typename callback<T>::tag_t;
        using func_t = typename callback<T>::func_t;

        static dispatcher& get()
        {
          static dispatcher d;
          return d;
        }

        void dispatch(T const &data)
        {
          for(auto it(m_handlers.begin()); it != m_handlers.end(); )
          {
            /* Callbacks must return whether or not to remain subscribed. */
            if((*it)(data) != state::subscribed)
            {
              if(it + 1 == m_handlers.end())
              {
                m_handlers.erase(it);
                break;
              }
              else
              { m_handlers.erase(it++); }
            }
            else
            { ++it; }
          }
        }

        tag_t subscribe(func_t const &callback)
        {
          m_handlers.push_back({ callback, ++m_tag_counter });
          return m_tag_counter;
        }

        void erase(tag_t const tag)
        {
          for(auto it(m_handlers.begin()); it != m_handlers.end(); ++it)
          {
            if(it->m_tag == tag)
            {
              m_handlers.erase(it);
              break;
            }
          }
        }

      private:
        dispatcher() = default;
        dispatcher(dispatcher const&) = delete;
        dispatcher(dispatcher &&) = delete;
        dispatcher& operator =(dispatcher const&) = delete;
        dispatcher& operator =(dispatcher &&) = delete;

        std::vector<val_t> m_handlers;
        uint64_t m_tag_counter{};
    };
  }
}

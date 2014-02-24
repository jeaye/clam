#pragma once

#include <string>
#include <utility>
#include <queue>
#include <thread>
#include <mutex>
#include <cstdint>

#include "dispatcher.h"

namespace shared
{
  namespace notif
  {
    template <typename Owner>
    class pool
    {
      public:
        using func_t = std::function<void ()>;

        static pool& global()
        {
          static pool ep;
          return ep;
        }

        template <typename T>
        void post(T const &event)
        {
          std::lock_guard<std::mutex> const lock(m_events_lock);
          m_events.push(std::bind(&dispatcher<Owner, T>::dispatch,
                                  std::ref(dispatcher<Owner, T>::get()), event));
        }

        /* std::function */
        template <typename A, typename R = state>
        typename callback<A>::tag_t subscribe(std::function<R (A)> const &cb)
        {
          static_assert(std::is_same<R, state>::value, "Callbacks must return a state or void");
          if(!cb)
          { throw std::invalid_argument("Invalid callback function"); }

          std::lock_guard<std::mutex> const lock(m_events_lock);
          return dispatcher<Owner, A>::get().subscribe(cb);
        }
        template <typename A>
        typename callback<A>::tag_t subscribe(std::function<void (A)> const &cb)
        { return subscribe<A, state>([=](A a){ cb(a); return state::subscribed; }); }

        /* Returns whether or not an event was handled. */
        bool poll()
        {
          func_t event;

          { /* No lock is needed while running the event; add some scope. */
            std::lock_guard<std::mutex> const lock(m_events_lock);

            if(m_events.empty())
            { return false; }

            event = m_events.front();
            m_events.pop();
          }

          event();
          return true;
        }

      private:
        std::queue<func_t> m_events;
        std::mutex m_events_lock;
    };
  }
}

/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: cpu.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "cpu.h"

#include <chrono>
#include <thread>
#include <random>

#ifdef __APPLE__
# include <mach/mach_init.h>
# include <mach/mach_error.h>
# include <mach/mach_host.h>
# include <mach/vm_map.h>
#endif

namespace shared
{
  namespace stat
  {
    static float pretend_cpu_load()
    {
      static float last{};
      static constexpr const float min{ 0.01371f };
      static constexpr const float max{ 0.51270f };
      static constexpr const float diff_min{ 0.0042f };
      static constexpr const float diff_max{ 0.018f };
      static std::random_device rd;
      static std::mt19937 gen(rd());
      static std::uniform_int_distribution<> dis(0, RAND_MAX);

      float diff{ ((diff_max - diff_min) *
                  (static_cast<float>(dis(gen)) / RAND_MAX)) + diff_min };
      diff *= (dis(gen) % 2) ? 1.0f : -1.0f;
      last += diff;
      last = std::max(last, min);
      last = std::min(last, max);
      return last;
    }

#ifdef __APPLE__
    static float calculate_cpu(uint64_t const idle, uint64_t const total)
    {
      static uint64_t previous_total{};
      static uint64_t previous_idle{};

      uint64_t const delta_total{ total - previous_total };
      uint64_t const delta_idle{ idle - previous_idle };
      float const ret{ (delta_total > 0) ?
                       (static_cast<float>(delta_idle)) / delta_total : 0 };
      previous_total = total;
      previous_idle = idle;

      return 1.0f - ret;
    }

    float cpu_load()
    {
      host_cpu_load_info_data_t cpuinfo;
      mach_msg_type_number_t count{ HOST_CPU_LOAD_INFO_COUNT };
      if(host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO,
                         reinterpret_cast<host_info_t>(&cpuinfo), &count) == KERN_SUCCESS)
      {
        uint64_t total{};
        for(int i{}; i < CPU_STATE_MAX; ++i)
        { total += cpuinfo.cpu_ticks[i]; }
        return calculate_cpu(cpuinfo.cpu_ticks[CPU_STATE_IDLE], total) * 100.0f;
      }
      else
      { return pretend_cpu_load() * 100.0f; }
    }
#else
    float cpu_load()
    {
      return pretend_cpu_load() * 100.0f;
    }
#endif

    std::string make_bar(size_t const width, float const percent)
    {
      if(width < 2)
      { return ""; }

      std::string bar(width, ' ');
      bar[0] = '['; bar[width - 1] = ']';
      for(size_t i{}; i < width - 2; ++i)
      {
        if(percent > i * (100.0f / (width - 2)))
        { bar[i + 1] = '|'; }
      }
      return bar;
    }

    std::string cpu_bar(size_t const width)
    { return make_bar(width, cpu_load()); }

#ifdef __APPLE__
    float free_ram()
    {
      mach_msg_type_number_t count{ HOST_VM_INFO_COUNT };
      vm_statistics_data_t vmstat;
      if(KERN_SUCCESS != host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmstat, &count))
      { return 0.0f; }

      double const total(vmstat.wire_count + vmstat.active_count + vmstat.inactive_count + vmstat.free_count);
      return (1.0f - (vmstat.free_count / total)) * 100.0f;
    }
#else
    float free_ram()
    { return 0.0f; }
#endif

    std::string ram_bar(size_t const width)
    { return make_bar(width, free_ram()); }
  }
}

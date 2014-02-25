#pragma once

#include "shared/notif/pool.h"

namespace server
{
  struct generic {};
  using generic_pool_t = shared::notif::pool<generic>;
}

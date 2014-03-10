/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: generic_pool.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "shared/notif/pool.h"

namespace server
{
  struct generic {};
  using generic_pool_t = shared::notif::pool<generic>;
}

/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: event.h
  Author: Jesse 'Jeaye' Wilkerson
*/

#pragma once

#include "cell.h"

namespace shared
{
  namespace term
  {
    struct resize_event
    {
      pos_t width, height;
      pos_t delta_width, delta_height;
    };

    enum class key
    {
      f1                = 0xFFFF - 0,
      f2                = 0xFFFF - 1,
      f3                = 0xFFFF - 2,
      f4                = 0xFFFF - 3,
      f5                = 0xFFFF - 4,
      f6                = 0xFFFF - 5,
      f7                = 0xFFFF - 6,
      f8                = 0xFFFF - 7,
      f9                = 0xFFFF - 8,
      f10               = 0xFFFF - 9,
      f11               = 0xFFFF - 10,
      f12               = 0xFFFF - 11,
      insert            = 0xFFFF - 12,
      del               = 0xFFFF - 13,
      home              = 0xFFFF - 14,
      end               = 0xFFFF - 15,
      pgup              = 0xFFFF - 16,
      pgdn              = 0xFFFF - 17,
      arrow_up          = 0xFFFF - 18,
      arrow_down        = 0xFFFF - 19,
      arrow_left        = 0xFFFF - 20,
      arrow_right       = 0xFFFF - 21,

      ctrl_tilde        = 0x00,
      ctrl_2            = 0x00, /* Clashes with ctrl_tilde */
      ctrl_a            = 0x01,
      ctrl_b            = 0x02,
      ctrl_c            = 0x03,
      ctrl_d            = 0x04,
      ctrl_e            = 0x05,
      ctrl_f            = 0x06,
      ctrl_g            = 0x07,
      backspace         = 0x08,
      ctrl_h            = 0x08, /* Clashes with ctrl_backspace */
      tab               = 0x09,
      ctrl_i            = 0x09, /* Clashes with tab */
      ctrl_j            = 0x0A,
      ctrl_k            = 0x0B,
      ctrl_l            = 0x0C,
      enter             = 0x0D,
      ctrl_m            = 0x0D, /* Clashes with enter */
      ctrl_n            = 0x0E,
      ctrl_o            = 0x0F,
      ctrl_p            = 0x10,
      ctrl_q            = 0x11,
      ctrl_r            = 0x12,
      ctrl_s            = 0x13,
      ctrl_t            = 0x14,
      ctrl_u            = 0x15,
      ctrl_v            = 0x16,
      ctrl_w            = 0x17,
      ctrl_x            = 0x18,
      ctrl_y            = 0x19,
      ctrl_z            = 0x1A,
      escape            = 0x1B,
      ctrl_lsq_bracket  = 0x1B, /* Clashes with esc */
      ctrl_3            = 0x1B, /* Clashes with esc */
      ctrl_4            = 0x1C,
      ctrl_backslash    = 0x1C, /* Clashes with ctrl_4 */
      ctrl_5            = 0x1D,
      ctrl_rsq_bracket  = 0x1D, /* Clashes with ctrl_5 */
      ctrl_6            = 0x1E,
      ctrl_7            = 0x1F,
      ctrl_slash        = 0x1F, /* Clashes with ctrl_7 */
      ctrl_underscore   = 0x1F, /* Clashes with ctrl_7 */
      space             = 0x20,
      backspace2        = 0x7F,
      ctrl_8            = 0x7F /* Clashes with delete */
    };

    enum class modifier
    {
      alt
    };

    /* If the unicode char is null, the value is
     * represented as a key code. */
    struct key_event
    {
      key k;
      unicode_t ch;
      modifier mod;
    };
  }
}

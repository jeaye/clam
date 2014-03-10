/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: main.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include <iostream>
#include <thread>
#include <chrono>

#include "core.h"
#include "shared/term/context.h"
#include "shared/term/window.h"

int main(int const, char ** const)
{
  try
  {
    /* TODO: Stream like rendering?
     *    term::stream << term::color::red << "error => " << term::color::off;
     *    term::stream << term::move{ 0,  10 } << "new, indented line";
     *    term::stream << term::move_rel{ 0,  10 } << "new, still indented line";
     *    term::stream.flush(); // present?
     *  window streams?
     *
     *  root window?
     *
     *  Titles too long? Cut off to show border
     */ 
    shared::term::context c;
    c.set_clear_color(shared::term::color_attrib::green,
                      shared::term::color_attrib::black);

    shared::term::window w;
    w.set_pos(1, 1);
    w.set_dimensions(c.get_width() - 2, c.get_height() - 2);
    std::string body{ "> " };
    shared::term::context::pool_t::global().subscribe<shared::term::key_event>(
    [&](shared::term::key_event const &ev)
    {
      if(ev.k == shared::term::key::escape)
      { throw std::runtime_error("escape pressed"); }
      if(ev.ch)
      { body += ev.ch; }
      else if(ev.k == shared::term::key::enter)
      { body += '\n'; }
      else if(ev.k == shared::term::key::backspace2 && body.size())
      { body.erase(body.end() - 1); }
    });
    shared::term::context::pool_t::global().subscribe<shared::term::resize_event>(
    [&](shared::term::resize_event const &ev)
    { w.set_width(ev.width - 2); w.set_height(ev.height - 2); });

    while(true)
    {
      c.clear();

      w.draw();
      w.draw(0, 0, body);

      c.present();
      c.wait_poll();
    }

    return 0;
    server::core core;
    core.run();
  }
  catch(std::exception const &e)
  { std::cout << "exception: " << e.what() << std::endl; }
  catch(...)
  { std::cout << "unknown error" << std::endl; }

  std::cout << "server done" << std::endl;
}

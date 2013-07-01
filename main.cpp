#include <xcb/xcb.h>
#include <X11/keysym.h>

#include "data_types.hpp"
#include "x_event_handler.hpp"
#include "x_connection.hpp"
#include "x_client.hpp"
#include "x_event_source.hpp"
#include "x_client_container.hpp"
#include "x_client_chooser.hpp"
#include "layout_t.hpp"
#include "grid.hpp"

int main(int argc, char ** argv)
{
  x_connection c;
  c.grab_key(XCB_MOD_MASK_4, XK_Tab);

  x_event_source es(c);
  x_client_container cc(c, es);

  grid_t grid;
  x_client_chooser cp(c, &grid, cc);

  es.register_handler(&c);
  es.register_handler(&cp);

  es.run_event_loop();

  return 0;
}
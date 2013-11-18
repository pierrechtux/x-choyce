#include <signal.h>
#include <X11/keysym.h>

#include "grid.hpp"
#include "thumbnail_manager.hpp"
#include "x_connection.hpp"
#include "x_client_chooser.hpp"
#include "x_client_thumbnail_gl.hpp"

x_event_source_t * g_event_source = NULL;

void sig_handler(int signum)
{
  if (g_event_source) g_event_source->shutdown();
}

int main(int argc, char ** argv)
{
  xcb_keysym_t east_key = XK_l;
  xcb_keysym_t west_key = XK_h;
  xcb_keysym_t north_key = XK_k;
  xcb_keysym_t south_key = XK_j;
  xcb_keysym_t quit_key = XK_Escape;
  xcb_keysym_t action_key = XK_Tab;
  xcb_mod_mask_t mod = XCB_MOD_MASK_4;

  x_connection c;
  g_event_source = &c;

  signal(SIGINT,  sig_handler);
  signal(SIGTERM, sig_handler);

  grid_t grid;

  x_client_thumbnail::factory factory;

  thumbnail_manager tm(c, &grid, &factory);
  x_client_chooser cp(c, &tm,
                      east_key, west_key, north_key, south_key,
                      quit_key, action_key, mod);

  c.run_event_loop();

  return 0;
}

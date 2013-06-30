#ifndef _X_CONNECTION
#define _X_CONNECTION

#include <string>
#include <vector>
#include <xcb/xcb.h>
#include <xcb/render.h>

#include "data_types.hpp"
#include "x_event_handler.hpp"

class x_connection : public x_event_handler {
  public:
    x_connection(void);
    ~x_connection(void);

    xcb_connection_t * operator()(void) const;

    void select_input(xcb_window_t window, xcb_event_mask_t event_mask) const;
    xcb_visualtype_t * default_visual_of_screen(void);
    void flush(void) const;
    xcb_screen_t * const default_screen(void) const;
    xcb_window_t const & root_window(void) const;
    uint8_t damage_event_id(void) const;
    void grab_key(uint16_t modifiers, xcb_keysym_t keysym) const;
    void grab_keyboard(void) const;
    void ungrab_keyboard(void) const;
    xcb_keysym_t keycode_to_keysym(xcb_keycode_t keycode) const;
    xcb_keycode_t keysym_to_keycode(xcb_keysym_t keysym) const;
    std::vector<xcb_window_t> net_client_list_stacking(void) const;
    xcb_atom_t intern_atom(const std::string & atom_name) const;
    xcb_window_t net_active_window(void) const;
    void request_change_current_desktop(unsigned int desktop_id) const;
    void request_change_active_window(xcb_window_t window) const;
    rectangle_t current_screen(void) const;
    void handle(xcb_generic_event_t * ge);
};


xcb_render_pictformat_t
render_find_visual_format(const x_connection & c, xcb_visualid_t visual);

xcb_render_picture_t
make_picture(const x_connection & c, xcb_window_t window);

xcb_visualtype_t *
argb_visual(const x_connection & c);

#endif

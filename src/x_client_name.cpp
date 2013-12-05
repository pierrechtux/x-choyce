#include "x_client_name.hpp"

#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_icccm.h>

x_client_name::x_client_name(x_connection & c,
                             x::xrm & xrm,
                             x_client & x_client)
  : m_c(c), m_xrm(xrm), m_x_client(x_client)
{
  m_c.attach(10, XCB_PROPERTY_NOTIFY, this);
  m_c.update_input(m_x_client.window(), XCB_EVENT_MASK_PROPERTY_CHANGE);

  m_xrm.attach(this);
  m_x_client.attach(this);

  load_config();

  update_wm_name();
  update_wm_class();
  update_net_wm_name();
}

x_client_name::~x_client_name(void)
{
  m_c.detach(XCB_PROPERTY_NOTIFY, this);
  m_xrm.detach(this);
  m_x_client.detach(this);
  xcb_free_pixmap(m_c(), m_title);
}

const std::string & x_client_name::net_wm_name(void) const
{
  return m_net_wm_name;
}

const std::string & x_client_name::wm_name(void) const
{
  return m_wm_name;
}

const std::string & x_client_name::wm_class_name(void) const
{
  return m_class_name;
}

const std::string & x_client_name::wm_instance_name(void) const
{
  return m_instance_name;
}

const xcb_pixmap_t &
x_client_name::title(void) const
{
  return m_title;
}

bool
x_client_name::handle(xcb_generic_event_t * ge)
{
  bool result = false;
  bool update_title = false;

  if (XCB_PROPERTY_NOTIFY == (ge->response_type & ~0x80)) {
    xcb_property_notify_event_t * e = (xcb_property_notify_event_t *)ge;

    if (e->window != m_x_client.window()) result = true;

    if (e->atom == m_a_wm_name) {
      update_title = true;
      update_wm_name();

    } else if (e->atom == m_a_wm_class) {
      update_title = true;
      update_wm_class();

    } else if (e->atom == m_a_net_wm_name) {
      update_title = true;
      update_net_wm_name();

    }

    result = true;
  }

  // TODO: cause clang to crash, FILE A BUG REPORT
#if defined  __GNUC__
  if (update_title) {
    observable::notify();
  }
#else
#error "Fix compilation with anything else but GNUC"
#endif

  return result;
}

void
x_client_name::notify(x::xrm *)
{
  load_config();
  // TODO: cause clang to crash, FILE A BUG REPORT
#if defined  __GNUC__
  observable::notify();
#else
#error "Fix compilation with anything else but GNUC"
#endif
}

void
x_client_name::notify(x_client *)
{
  // TODO: cause clang to crash, FILE A BUG REPORT
#if defined  __GNUC__
  observable::notify();
#else
#error "Fix compilation with anything else but GNUC"
#endif
}

// private

void
x_client_name::load_config(void)
{
  m_icon_size    = m_xrm["iconsize"].v.num;
  m_border_width = m_xrm["borderwidth"].v.num;
  m_pnamefont    = *m_xrm["titlefont"].v.str;
  m_titlefont    = *m_xrm["subtitlefont"].v.str;
  m_colorname    = *m_xrm["titlefgcolor"].v.str;

  m_title_bg_color =
    std::strtol(m_xrm["titlebgcolor"].v.str->substr(1,6).c_str(), NULL, 16);

  m_title_bg_color |= (int)(0xff * m_xrm["titlebgalpha"].v.dbl) << 24;
}

void
x_client_name::update_net_wm_name(void)
{
  m_net_wm_name.clear();

  xcb_generic_error_t * error;
  xcb_get_property_cookie_t c =
    xcb_ewmh_get_wm_name(m_c.ewmh(), m_x_client.window());
  xcb_get_property_reply_t * r = xcb_get_property_reply(m_c(), c, &error);

  if (error) {
    delete error;

  } else {
    xcb_ewmh_get_utf8_strings_reply_t net_wm_name;
    if (xcb_ewmh_get_wm_name_from_reply(m_c.ewmh(), &net_wm_name, r)) {
      m_net_wm_name = std::string(net_wm_name.strings, net_wm_name.strings_len);
      xcb_ewmh_get_utf8_strings_reply_wipe(&net_wm_name);
      r = NULL;
    }
  }

  if (r) delete r;
}

void
x_client_name::update_wm_name(void)
{
  m_wm_name.clear();

  xcb_generic_error_t * error;
  xcb_icccm_get_text_property_reply_t wm_name;
  xcb_get_property_cookie_t c =
    xcb_icccm_get_wm_name(m_c(), m_x_client.window());
  xcb_icccm_get_wm_name_reply(m_c(), c, &wm_name, &error);

  // TODO: cause clang to crash, FILE A BUG REPORT
#if defined  __GNUC__
  if (error) {
    delete error;

  } else {
    m_wm_name = std::string(wm_name.name, wm_name.name_len);
    xcb_icccm_get_text_property_reply_wipe(&wm_name);
  }

#else
#error "Fix compilation with anything else but GNUC"
#endif
}

void
x_client_name::update_wm_class(void)
{
  m_class_name.clear();
  m_instance_name.clear();

  xcb_generic_error_t * error;
  xcb_get_property_cookie_t c =
    xcb_icccm_get_wm_class(m_c(), m_x_client.window());
  xcb_get_property_reply_t * r = xcb_get_property_reply(m_c(), c, &error);

  if (error) {
    delete error;

  } else {
    xcb_icccm_get_wm_class_reply_t wm_class;
    if (xcb_icccm_get_wm_class_from_reply(&wm_class, r)) {
      m_class_name = wm_class.class_name;
      m_instance_name = wm_class.instance_name;
      xcb_icccm_get_wm_class_reply_wipe(&wm_class);
      r = NULL;
    }
  }

  if (r) delete r;
}

void
x_client_name::make_title(void)
{
  xcb_free_pixmap(m_c(), m_title);

  m_title = xcb_generate_id(m_c());
  xcb_create_pixmap(m_c(), 32, m_title, m_c.root_window(),
                    m_title_width, m_title_height);

  xcb_gcontext_t gc = xcb_generate_id(m_c());
  xcb_create_gc(m_c(), gc, m_title, XCB_GC_FOREGROUND, &m_title_bg_color );
  xcb_rectangle_t r = { 0, 0, (uint16_t)m_title_width, (uint16_t)m_title_height };
  xcb_poly_fill_rectangle(m_c(), m_title, gc, 1, &r);
  xcb_free_gc(m_c(), gc);

  m_x_xft = std::shared_ptr<x::xft>(new x::xft(m_c.dpy(), m_title, 32));

  std::string pname = m_class_name;
  std::string title = m_net_wm_name.empty() ? m_wm_name : m_net_wm_name;

  std::string lower = "abcdefghijklmnopqrstuvwxy";
  std::string upper = "ABCDEFGHIJKLMNOPQRSTUVWXY";

  XGlyphInfo text_extents = m_x_xft->text_extents_utf8(m_pnamefont, lower + upper);

  int x_off = m_icon_size + 4 * m_border_width;
  int y_off = m_border_width;

  y_off += text_extents.height;
  m_x_xft->draw_string_utf8(pname, x_off, y_off, m_pnamefont, m_colorname);

  y_off += text_extents.height;
  m_x_xft->draw_string_utf8(title, x_off, y_off, m_titlefont, m_colorname);
}

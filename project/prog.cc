#include <xcb/xcb.h>
#include <stdio.h>
#include <unistd.h>

int main () {
   xcb_connection_t *c;
  xcb_screen_t     *screen;
  xcb_drawable_t    win;
  xcb_gcontext_t    foreground;
  uint32_t          mask;
  uint32_t          value[1];

  /* Open the connection to the X server and get the first screen */
  c = xcb_connect (NULL, NULL);
  screen = xcb_setup_roots_iterator (xcb_get_setup (c)).data;

  /* Create a black graphic context for drawing in the foreground */
  win = screen->root;
  black = xcb_generate_id (c);
  mask = XCB_GC_FOREGROUND;
  value[0] = screen->white_pixel;
  xcb_create_gc (c, black, win, mask, value);

    /* Ask for our window's Id */
  win = xcb_generate_id(c);


  /* Create the window */
  xcb_create_window (c,                             /* Connection          */
                     XCB_COPY_FROM_PARENT,          /* depth (same as root)*/
                     win,                           /* window Id           */
                     screen->root,                  /* parent window       */
                     50, 50,                          /* x, y                */
                     150, 150,                      /* width, height       */
                     0,                            /* border_width        */
                     XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                     screen->root_visual,           /* visual              */
                     0, NULL);                      /* masks, not used yet */

  /* Map the window on the screen */
  xcb_map_window (c, win);

  /* Make sure commands are sent before we pause, so window is shown */
  xcb_flush (c);

  pause ();    /* hold client until Ctrl-C */

  xcb_disconnect(c);

  return 0;
}
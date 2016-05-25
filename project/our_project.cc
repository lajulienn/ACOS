#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <xcb/xproto.h>
#include <xcb/xcb.h>
#include <iostream>
#include <math.h>

#define POINTER_LENGTH 50
#define H_LENGTH 10
#define HALF_S_LENGTH 6
 
struct Point{
  double x;
  double y;
} cursor,begin_; 

double get_length(Point a, Point b){
  return sqrt((b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y));
}

Point segment_division(Point a, Point b, double lambda){
  Point res;
  res.x = (a.x + lambda*b.x)/(1+lambda);
  res.y = (a.y + lambda*b.y)/(1+lambda);
  return res;
}

void set_triangle(xcb_point_t *triangle){
  Point end;
  double lambda;
  lambda = (double) POINTER_LENGTH/(get_length(begin_ , cursor) - POINTER_LENGTH);
  end = segment_division(begin_, cursor, lambda);
  Point h;
  lambda = (POINTER_LENGTH - H_LENGTH)/H_LENGTH;
  h = segment_division(begin_, end, lambda);
  double sin_a, cos_a;
  sin_a = (double)end.y/POINTER_LENGTH;
  cos_a = (double)end.x/POINTER_LENGTH;
  Point a,b;
  a.x = h.x + HALF_S_LENGTH*sin_a;
  a.y = h.y - HALF_S_LENGTH*cos_a;
  b.x = h.x - HALF_S_LENGTH*sin_a;
  b.y = h.y + HALF_S_LENGTH*cos_a;
  triangle[0].x = end.x;
  triangle[0].y = end.y;
  triangle[1].x = a.x;
  triangle[1].y = a.y;
  triangle[2].x = b.x;
  triangle[2].y = b.y;
}
void set_segment(xcb_segment_t* segment){
  Point end;
  double lambda;
  lambda = (double)POINTER_LENGTH/(get_length(begin_ , cursor) - POINTER_LENGTH);
  end = segment_division(begin_, cursor, lambda);
  segment[0].x1 = begin_.x;
  segment[0].y1 = begin_.y;
  segment[0].x2 = end.x;
  segment[0].y2 = end.y;
}

int main(){
  begin_.x = 50;
  begin_.y = 50;
  xcb_connection_t    *c;
  xcb_screen_t        *screen;
  xcb_drawable_t       win;
  xcb_gcontext_t       foreground;
  xcb_generic_event_t *e;
  uint32_t             mask = 0;
  uint32_t             values[2];

  /* Open the connection to the X server */
  c = xcb_connect (NULL, NULL);

  /* Get the first screen */
  screen = xcb_setup_roots_iterator (xcb_get_setup (c)).data;
  /* Create black (foreground) graphic context */
  win = screen->root;
  foreground = xcb_generate_id (c);
  // while(true){
  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = screen->black_pixel;
  values[1] = 0;
  xcb_create_gc (c, foreground, win, mask, values);

  /* Ask for our window's Id */
  win = xcb_generate_id(c);
  
  /* Create the window */
  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = screen->white_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE;
  xcb_create_window (c,                             /* Connection          */
                     XCB_COPY_FROM_PARENT,          /* depth               */
                     win,                           /* window Id           */
                     screen->root,                  /* parent window       */do
                     0, 0,                          /* x, y                */
                     150, 150,                      /* width, height       */
                     10,                            /* border_width        */
                     XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                     screen->root_visual,           /* visual              */
                     mask, values);                 /* masks */


  /* Map the window on the screen */
  xcb_map_window (c, win);
  /* We flush the request */
  xcb_flush (c);
  
  xcb_query_pointer_cookie_t cookie;
  xcb_query_pointer_reply_t* rep;

  
  cookie = xcb_query_pointer(c, win);
  rep = xcb_query_pointer_reply(c,cookie, 0 );
  
  cursor.x = rep->root_x - 40;
  cursor.y = rep->root_y - 60;

  std::cout<<cursor.x<<"\n";  
  std::cout<<cursor.y<<"\n";

  xcb_point_t triangle[] = {
    {0, 0},
    {0, 0},
    {0, 0}
  };

  set_triangle(triangle);
  xcb_segment_t segments[] = {
    {0,0,0,0}
  };
  set_segment(segments);
  /*std::cout<<triangle[0].x<<" "<<triangle[0].y<<"\n";
  std::cout<<triangle[1].x<<" "<<triangle[1].y<<"\n";
  std::cout<<triangle[2].x<<" "<<triangle[2].y<<"\n";

  std::cout<<segments[0].x1<<" "<<segments[0].y1<<"\n";
  std::cout<<segments[0].x2<<" "<<segments[0].y2<<"\n";
  */
  
    while (e = xcb_wait_for_event (c)){
    switch (e->response_type & ~0x80) {
    case XCB_EXPOSE: {
      /* We draw the segements */
      xcb_poly_segment (c, win, foreground, 2, segments);
      /* We draw triangle*/
      xcb_fill_poly(c, win, foreground,XCB_POLY_SHAPE_CONVEX, XCB_COORD_MODE_ORIGIN, 3, triangle );
      /* We flush the request */
      xcb_flush (c);
      break;
    }
    default: {
      /* Unknown event type, ignore it */
      break;
    }
    }
    /* Free the Generic Event */
    free (e);
  }
  
  return 0;
}
#include <xcb/xcb.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <xcb/xproto.h>
#include <stdlib.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

struct Point {
  GLfloat x;
  GLfloat y;
};

int win_pos_x = 70; 
int win_pos_y = 50; 
const int win_width = 150;
const int win_height = 150;

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void draw_line(Point p1, Point p2);
void draw_triangle(Point p1, Point p2, Point p3);
double radian_to_degree(double angle);
double get_angle(Point cursor, Point root_axis);
void window_pos_callback(GLFWwindow* window, int xpos, int ypos);
void set_axis_coordinates(Point &root_axis);

int main(void) {
  // initializing points
  Point win_axis; // axis in window coordinates
  win_axis.x = 0.0f;
  win_axis.y = 0.0f;

  Point root_axis; // axis in device coordinates
  set_axis_coordinates(root_axis);

  Point pointer_end;
  pointer_end.x = 0.9f;
  pointer_end.y = 0.0f;

  Point tr_vertex1;
  tr_vertex1.x = 0.8f;
  tr_vertex1.y = 0.1f;

  Point tr_vertex2;
  tr_vertex2.x = 0.8f;
  tr_vertex2.y = -0.1f;

  // working with xcb
  xcb_connection_t    *c;
  xcb_screen_t        *screen;
  c = xcb_connect (NULL, NULL);
  if (xcb_connection_has_error(c)) {
    perror("Failed to connect to the X server\n");
    exit(EXIT_FAILURE);
  }
  screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;

  // creating window
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    perror("Failed to initialize GLFW library\n");
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_DECORATED, GL_FALSE);

  window = glfwCreateWindow(win_width, win_height, "Pointer", NULL, NULL);
  if (!window){
    perror("Failed to create a window\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwSetKeyCallback(window, key_callback);
  glfwSetWindowPosCallback(window, window_pos_callback);
  glfwSetWindowPos(window, win_pos_x, win_pos_y);

  while (!glfwWindowShouldClose(window)){
    // geting cursor position
    xcb_query_pointer_cookie_t cookie;
    xcb_query_pointer_reply_t* rep;
    cookie = xcb_query_pointer(c, screen->root);
    rep = xcb_query_pointer_reply(c, cookie, 0);
    Point cursor;
    cursor.x = rep->root_x;
    cursor.y = rep->root_y;
   
    set_axis_coordinates(root_axis);
    double angle = get_angle(cursor, root_axis);

    // drawing 
    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(angle, 0.f, 0.f, 1.f);

    draw_line(win_axis, pointer_end); 
    draw_triangle(pointer_end, tr_vertex1, tr_vertex2);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  xcb_disconnect(c);
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

static void error_callback(int error, const char* description) {
  fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

void draw_line(Point p1, Point p2 ){
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(2.0);
  glBegin(GL_LINES);
  glColor3f(1.0, 0.0, 0.0);
  glVertex2f(p1.x, p1.y);
  glVertex2f(p2.x, p2.y);
  glEnd(); 
}

void draw_triangle(Point p1, Point p2, Point p3) {
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(2.0);
  glBegin(GL_TRIANGLES);
  glColor3f(1.0, 0.0, 0.0);
  glVertex2f(p1.x, p1.y);
  glVertex2f(p2.x, p2.y);
  glVertex2f(p3.x, p3.y);
  glEnd();
}

double radian_to_degree(double angle) {
  return angle * 360 / (2 * M_PI);
}

double get_angle(Point cursor, Point root_axis) {
  GLfloat x = fabs(cursor.x - root_axis.x);
  GLfloat y = fabs(cursor.y - root_axis.y);
  double angle;
  if((cursor.x >= root_axis.x) && (cursor.y <= root_axis.y)){// first quater
    angle = atan(y / x);  
  } else if((cursor.x < root_axis.x) && (cursor.y <= root_axis.y)){// second quater
    angle = M_PI - atan(y / x);
  } else if((cursor.x < root_axis.x) && (cursor.y > root_axis.y)){ // third quater
    angle = M_PI + atan(y / x);
  } else if((cursor.x >= root_axis.x) && (cursor.y > root_axis.y)){ // fourth quater
    angle = 2 * M_PI - atan(y /x);
  }
 return radian_to_degree(angle); 
}


void window_pos_callback(GLFWwindow* window, int xpos, int ypos) {
  win_pos_x = xpos;
  win_pos_y = ypos;
}
void set_axis_coordinates(Point &root_axis) {
  root_axis.x = win_pos_x + win_width / 2;
  root_axis.y = win_pos_y + win_height / 2;
} 

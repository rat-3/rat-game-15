#ifndef RATATOUILLE
#define RATATOUILLE 0.0
// #include <string>
#include <curses.h>
#include <functional>
typedef NCURSES_SIZE_T nsize;
namespace ui {
  enum border_type {
    VERTICAL,
    HORIZONTAL,
    CORNER,
  };
  class component {
    WINDOW* win;//need a way to localize this to the ncurses version
    public:
    nsize& x0,y0,x1,y1;
    nsize last_x0,last_y0,last_x1,last_y1;
    const char* text;//disallow any non printing except \n
    std::function<char(border_type,nsize)> borderprovider;
    component(const char* text,nsize height,nsize width,nsize y,nsize x);
    component(void);//i like having the little void in there it looks nice and neat
   ~component(void);
    void draw(void);//borders n text or whatever
    void refresh(void);
  };
  void init(void);
  void stop(void);
}
#endif
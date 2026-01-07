#include <curses.h>//c++ curses is a fuck: tries to include etip.h when it's etip.h.in??? which is stupid
#include <iostream>//anyways r@@ui is now a bridge to whatever graphics shit we're doing. on linux it'll
#include <cassert>
#include <cstring>
#include <r@@ui.h>// be to c (n)curses. this is for multiplatform viability. will write shell scripts for whatever
#include <functional>

namespace ui {//https://pubs.opengroup.org/onlinepubs/007908799/xcurses/curses.h.html
  WINDOW* mainWin;
  struct {
    unsigned int scractive:1;//one whole bit
  } state = {
    //0-initialization
  };
  std::function<char(border_type,nsize)> defaultborderprovider([](border_type t,nsize i){
    switch(t){
      case CORNER:return '+';
      case VERTICAL:return '|';
      case HORIZONTAL:return '-';
    }
    return '?';
  });
  component::component(const char* content,nsize height,nsize width,nsize y,nsize x):
    win(newwin(height,width,y,x)),
    text(content),
    x0(win->_begx),y0(win->_begy),x1(win->_maxx),y1(win->_maxy)
    ,borderprovider(defaultborderprovider)
    // ,last_x0(x0),last_y0(y0),last_x1(x1),last_y1(y1)
  {}
  component::component():component(NULL,4,16,3,3){}
  component::~component(){
    delwin(win);
  }
  void component::draw(){
    mvwaddch(win,0,0,borderprovider(CORNER,0));
    mvwaddch(win,0,x1,borderprovider(CORNER,1));
    mvwaddch(win,y1,0,borderprovider(CORNER,2));
    mvwaddch(win,y1,x1,borderprovider(CORNER,3));
    unsigned int selstart=0;
    unsigned int y=1;
    unsigned int length=strlen(text);
    for(unsigned int i=1;i<length;i++){
      if(i==selstart){continue;}
      if( (text[i]=='\n') || !( (i-selstart) % (x1-1) ) ){//should we break line
        mvwaddnstr(win,y,1,&text[selstart],i-selstart);
        if(text[i]!='\n'){//this could look a lot nicer
          y++;
        }else{
          while(text[i]=='\n'){i++;y++;}//refuse to print newline chars but advance regardess
        }
        selstart=i;
      }
    }
    mvwaddstr(win,y,1,&text[selstart]);
  }
  void component::refresh(){
    wrefresh(win);
  }
  void init(){
    mainWin = initscr();
    cbreak();
    noecho();
    curs_set(0);
    refresh();
    state.scractive=1;
  }
  void stop(){
    state.scractive=0;
    endwin();
  }
}
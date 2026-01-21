#ifndef RATATOUILLE_NCURSES
#define RATATOUILLE_NCURSES "yello"
#include <curses.h>//c++ curses is a fuck: tries to include etip.h when it's etip.h.in??? which is stupid
#include <iostream>//anyways r@@ui is now a bridge to whatever graphics shit we're doing. on linux it'll
#include <cassert>
#include <cstring>
#include <r@@ui.h>// be to c (n)curses. this is for multiplatform viability. will write shell scripts for whatever
#include <functional>
#include <cmath>
namespace render {
  std::vector<tri3<float>> map;
  double fov=M_PI/2.0;
  double fov1=tan(fov/2.0);
}
namespace ui {//https://pubs.opengroup.org/onlinepubs/007908799/xcurses/curses.h.html
  using namespace render;
  WINDOW* mainWin;
  char(*defaultborderprovider)(border_type,rat_size) noexcept=[](border_type t,rat_size i)noexcept->char{
    switch(t){
      case CORNER:return '+';
      case VERTICAL:return '|';
      case HORIZONTAL:return '-';
    }
    return '?';
  };

  component::component(const component& c) noexcept:
    c_win(newwin(c.y_1,c.x_1,c.y_0,c.x_0)),
    title(strcpy(new char[strlen(c.title)+1],c.title)),
    x_0((rat_size&)c_win->_begx),y_0(c_win->_begy),x_1(c_win->_maxx),y_1(c_win->_maxy),
    borderprovider(c.borderprovider)
    // ,last_x0(x_0),last_y0(y_0),last_x1(x_1),last_y1(y_1)
  {}
  textcomponent::textcomponent(const component& c) noexcept:
    component(c){}
  textcomponent::textcomponent(const textcomponent& tc) noexcept:
    component(tc),
    text(strcpy(new char[strlen(tc.text)+1],tc.text)){}//switch all these to mallocs and frees if im being real
  cameracomponent::cameracomponent(const component& c) noexcept:
    component(c){}//shouldn't need another one because no extra variables

  component::component() noexcept:component("",4,16,3,3){}
  textcomponent::textcomponent() noexcept:textcomponent("","",4,16,3,3){}

  component::~component() noexcept {delwin(c_win);}
  textcomponent::~textcomponent() noexcept {/*this destructor called, then parent class*/}
  cameracomponent::~cameracomponent() noexcept{}

  void component::corner() const noexcept {
    wattron(c_win,COLOR_PAIR(0));
    mvwaddch(c_win,0,0,borderprovider(CORNER,0));
    mvwaddch(c_win,0,x_1,borderprovider(CORNER,1));
    mvwaddch(c_win,y_1,0,borderprovider(CORNER,2));
    mvwaddch(c_win,y_1,x_1,borderprovider(CORNER,3));
    if((title!="")&&(strlen(title)>x_1-2)){
      mvwaddnstr(c_win,0,1,title,x_1-4);
      mvwaddstr(c_win,0,x_1-3,"...");
    }else{
      mvwaddstr(c_win,0,1,title);
    }
  }

  void component::draw() const noexcept {corner();}
  void textcomponent::draw() const noexcept {
    corner();
    unsigned int selstart=0;
    unsigned int y=1;
    unsigned int x=0;
    unsigned int length=strlen(text);
    for(unsigned int i=1;i<=length;i++){
      if((text[i]==' ')||(i==length)||(text[i]=='\n')){
        if((x+(i-selstart))>(x_1-1)){
          y++;x=0;
          selstart++;
        }
        mvwaddnstr(c_win,y,x+1,&text[selstart],i-selstart);
        if(text[i]=='\n'){
          y++;i++;
          x=0;
        }else{
          x+=i-selstart;
        }
        selstart=i;
      }
    }
  }
  void cameracomponent::draw() const noexcept {
    wclear(c_win);
    wattron(c_win,COLOR_PAIR(0));
    corner();
    // std::cout<<fov1<<'\n';
    std::for_each(map.begin(),map.end(),[this](tri3<float> l){
      l.a=l.a-cPos;l.b=l.b-cPos;l.c=l.c-cPos;
      rot(l.a,cRot);rot(l.b,cRot);rot(l.c,cRot);
      drawTri(toScreenSpace(l,x_1,y_1),1,'#');
    });
  }

  void component::refresh() const noexcept {wrefresh(c_win);}

  // void component::focus(){state.focused=&this;}

  void cameracomponent::putPixel(rat_size x,rat_size y,char color,char c) const {
    wattron(c_win,COLOR_PAIR(color));
    mvwaddch(c_win,y,x,c);
  }

  void cameracomponent::drawLine(rat_size x0,rat_size y0,rat_size x1,rat_size y1,char color) const {
      short signed int m1=y0-y1,m2=x0-x1;
      short unsigned int am1=abs(m1),am2=abs(m2);
      if(am1<am2){
        char c=(m1==0?'-':((m1>0)^(m2>0))?'/':'\\'),c1=0;
        float m=static_cast<float>(m1)/m2,a=y0;
        signed char s=(m2>0?-1:1);
        short signed int y=y0;
        for(short signed int i=x0;i!=x_1;i+=s){
          a=((i-x0)*m+y0);
          if((((int)a)!=y)||((int)((i-x0+s)*m+y0)!=y)){c1=c;}else{c1='-';}
          y=(int)a;
          putPixel(i,y,color,c1);
        }
      }else{
        char c=(m2==0?'|':((m2>0)^(m1>0))?'/':'\\'),c1=0;
        float m=static_cast<float>(m2)/m1,a=x0;
        signed char s=(m1>0?-1:1);
        short signed int x=x0;
        for(short signed int i=y0;i!=y1;i+=s){
          a=((i-y0)*m+x0);
          if((((int)a)!=x)||((int)((i-y0+s)*m+x0)!=x)){c1=c;}else{c1='|';}
          x=(int)a;
          putPixel(x,i,color,c1);
        }
      }
    // }
  }
  void cameracomponent::drawTri(rat_size x0,rat_size y0,rat_size x1,rat_size y1,rat_size x2,rat_size y2,char color, char ch) const {
    mvwprintw(c_win,1,0,"%i",triarea(x0,y0,x1,y1,x2,y2));
    rat_size minx=std::min(std::min(x0,x1),x2),
             miny=std::min(std::min(y0,y1),y2),
             maxx=std::max(std::max(x0,x1),x2),
             maxy=std::max(std::max(y0,y1),y2);
    for(rat_size x=minx;x<maxx;x++){
      for(rat_size y=miny;y<maxy;y++){
        if(triarea(x,y,x1,y1,x2,y2)>=0){if(triarea(x0,y0,x,y,x2,y2)>=0){if(triarea(x0,y0,x1,y1,x,y)>=0){
          putPixel(x,y,2,'#');
        }}}
      }
    }
    putPixel(x0,y0,1,'+');
    putPixel(x1,y1,1,'+');
    putPixel(x2,y2,1,'+');
  }
  void init() noexcept {
    mainWin = initscr();
    cbreak();
    keypad(mainWin,TRUE);
    noecho();
    curs_set(0);
    refresh();
    state.screen_height=mainWin->_maxy;state.screen_width=mainWin->_maxx;
    state.scractive=1;
  }

  void stop() noexcept {
    state.scractive=0;
    endwin();
  }
}

namespace render {
  void genMap(){
    map.push_back(
      (tri3<float>){
        (vec3<float>){5, 1, 1},
        (vec3<float>){5,-1, 1},
        (vec3<float>){5, 1,-1}
      });
    /*
    map.push_back(
      (lin3<float>){
        (vec3<float>){5,1,1},
        (vec3<float>){5,1,-1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){5,1,-1},
        (vec3<float>){5,-1,-1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){5,-1,-1},
        (vec3<float>){5,-1,1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){5,-1,1},
        (vec3<float>){4,-1,1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){4,-1,1},
        (vec3<float>){4,-1,-1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){4,-1,-1},
        (vec3<float>){4,1,-1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){4,1,-1},
        (vec3<float>){4,1,1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){4,1,1},
        (vec3<float>){5,1,1}
      }
    );
    */
  }

  void init(){
    assert(::ui::state.scractive);//idiot
    assert(has_colors());//we need color :/
    start_color();
    init_pair(0,COLOR_WHITE,COLOR_BLACK);
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    init_pair(2,COLOR_RED,COLOR_BLACK);
    attron(COLOR_PAIR(0));
    // wprintw(ui::mainWin,"max pairs=%u",COLOR_PAIRS);
    genMap();
  }
  void stop(){
    //kill
  }
}
#endif
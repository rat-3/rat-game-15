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
  std::vector<lin3<float>> map;
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
    c_win(newwin(c.y1,c.x1,c.y0,c.x0)),
    title(strcpy(new char[strlen(c.title)+1],c.title)),
    x0((rat_size&)c_win->_begx),y0(c_win->_begy),x1(c_win->_maxx),y1(c_win->_maxy),
    borderprovider(c.borderprovider)
    // ,last_x0(x0),last_y0(y0),last_x1(x1),last_y1
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
    mvwaddch(c_win,0,x1,borderprovider(CORNER,1));
    mvwaddch(c_win,y1,0,borderprovider(CORNER,2));
    mvwaddch(c_win,y1,x1,borderprovider(CORNER,3));
    if((title!="")&&(strlen(title)>x1-2)){
      mvwaddnstr(c_win,0,1,title,x1-4);
      mvwaddstr(c_win,0,x1-3,"...");
    }else{
      mvwaddstr(c_win,0,1,title);
    }
  }

  void component::draw() const noexcept {corner();mvwaddnstr(c_win,1,1,"empty component",x1-1);}
  void textcomponent::draw() const noexcept {
    corner();
    unsigned int selstart=0;
    unsigned int y=1;
    unsigned int x=0;
    unsigned int length=strlen(text);
    for(unsigned int i=1;i<=length;i++){
      if((text[i]==' ')||(i==length)||(text[i]=='\n')){
        if((x+(i-selstart))>(x1-1)){
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
    std::for_each(map.begin(),map.end(),[this](lin3<float> l){
      l.a=l.a-cPos;l.b=l.b-cPos;
      // if((l.a.x>0)&&(l.b.x>0)){
        rot(l.a,cRot);rot(l.b,cRot);
        vec2<rat_size> a={
          .x=(rat_size)((l.a.y/l.a.x+1)/2*x1),
          .y=(rat_size)((l.a.z/l.a.x+1)/2*y1)
        },b={
          .x=(rat_size)((l.b.y/l.b.x+1)/2*x1),
          .y=(rat_size)((l.b.z/l.b.x+1)/2*y1)
        };
        drawLine(a,b,2);
        putPixel(a,1,'+');
        putPixel(b,1,'+');
      // }
    });
  }

  void component::refresh() const noexcept {wrefresh(c_win);}

  // void component::focus(){state.focused=&this;}

  void cameracomponent::putPixel(vec2<integral auto> p,char color,char c) const {
    wattron(c_win,COLOR_PAIR(color));
    mvwaddch(c_win,p.y,p.x,c);
  }
  
  void cameracomponent::drawLine(vec2<integral auto> a,vec2<integral auto> b,char color) const {
    if((a.x<x1)&&(a.y<y1)&&(b.x<x1)&&(b.y<y1)){
      short signed int m1=a.y-b.y,m2=a.x-b.x;
      short unsigned int am1=abs(m1),am2=abs(m2);
      mvwprintw(mainWin,30,0,"(%u,%u)->(%u,%u)",a.x,a.y,b.x,b.y);
      if(am1<am2){
        char c=(m1==0?'-':((m1>0)^(m2>0))?'/':'\\'),c1=c;
        float m=static_cast<float>(m1)/m2,y1=a.y;
        signed char s=(m2>0?-1:1);
        short signed int y=a.y;
        for(short signed int i=a.x;i!=b.x;i+=s){
          y1=((i-a.x)*m+a.y);
          if((((int)y1)!=y)||((int)((i-a.x+s)*m+a.y)!=y)){c1=c;}else{c1='-';}
          y=(int)y1;
          putPixel((vec2<int>){i,y},color,c1);
        }
      }else{
        char c=(m2==0?'|':((m1>0)^(m2>0))?'\\':'/');
        float m=static_cast<float>(m2)/m1;
        signed char s=(m1>0?-1:1);
        for(short signed int i=a.y;i!=b.y;i+=s){
          putPixel((vec2<int>){(int)(i*m+a.x),i},color,c);
        }
      }
    }
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
      (lin3<float>){
        (vec3<float>){1,1,1},
        (vec3<float>){1,1,-1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){1,1,-1},
        (vec3<float>){1,-1,-1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){1,-1,-1},
        (vec3<float>){1,-1,1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){1,-1,1},
        (vec3<float>){-1,-1,1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){-1,-1,1},
        (vec3<float>){-1,-1,-1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){-1,-1,-1},
        (vec3<float>){-1,1,-1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){-1,1,-1},
        (vec3<float>){-1,1,1}
      }
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){-1,1,1},
        (vec3<float>){1,1,1}
      }
    );
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
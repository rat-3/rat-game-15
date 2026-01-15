#define MAIN ":3"
#define RATATOUILLE_NCURSES "terminal illness"
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <r@@ui.h>
int main(int argc,char *argv[]){
  // std::cout<<
  // exit(0);
  ui::init();
  render::init();
  ui::cameracomponent wow("camera test",24,100,4,0);
  ui::textcomponent greg(new char[60],"debug",4,32,0,0);
  int i=0;
  render::vec3<float> pjeanort{0,0,0};
  while(i!='\\'){
    switch(i){
      case 'w':pjeanort.x+=1;break;
      case 'd':pjeanort.y+=1;break;
      case 'a':pjeanort.y-=1;break;
      case 's':pjeanort.x-=1;break;
      // case 258:down;break;
      // case 259:up;break;
      case 260:wow.cRot+=16;break;
      case 261:wow.cRot-=16;break;
    }
    // render::rot(pjeanort,wow.cRot);
    snprintf(greg.text,60,"(%-#0.3f,%-#0.3f) ",pjeanort.x,pjeanort.y);
    wow.cPos=wow.cPos+pjeanort;
    pjeanort.x=0;pjeanort.y=0;
    wow.draw();
    greg.draw();
    wow.refresh();
    greg.refresh();
    i=getch();
  }
  ui::stop();
  return 0;
}

#define MAIN
// #include <util.h>
// #include <math.h>
#include <r@@ui.h>
// #include <ctype.h>
#include <stdio.h>
// #include <curses.h>
// #include <assets.h>
// #include <stdlib.h>
// #include <string.h>
// #include <pthread.h>
int main(int argc,char *argv[]){
  ui::init();
  ui::component hello("hello world,\nwe have line wrapping\n\n\nwith breaks",8,16,2,2);
  hello.draw();
  hello.refresh();
  getch();
  ui::stop();
  return 0;
}
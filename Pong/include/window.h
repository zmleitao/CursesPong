#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

int win_x;
int win_y;
int win_height;
int win_width;
WINDOW *win;

void   WindowInit();
WINDOW *  GameWindowInit();
WINDOW *create_newwin(int height, int width, int starty, int startx, bool border);
void print_middle(WINDOW *win,int starty,int startx,char *s);
void WinRefresh(WINDOW *win);

#endif


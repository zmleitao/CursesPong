#ifndef _ITEMS_H_
#define _ITEMS_H_

#include <ncurses.h>

#define POSX 10 //initial position x
#define POSY 15 //initial position y
#define DIRX -1 //initial direction x
#define DIRY -1 //initial direction y



typedef struct position_{ 
	int x;
	int y;
}position;

typedef struct ball{
	unsigned int speed; //ball speed corresponds to the time interval between consecutive movements
	position dir; //direction in which the ball is moving
	position pos;
}ball_t;


void new_ball(ball_t *ball, unsigned int speed,WINDOW *game_win);
void move_ball(WINDOW *game_win, WINDOW *win, ball_t *ball);

#endif




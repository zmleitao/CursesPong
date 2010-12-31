#include "items.h"
#include "window.h"
/*Creates and prints a new ball*/
void new_ball(ball_t *ball, unsigned int speed,WINDOW *game_win){
	ball->speed = speed;
	(ball->dir).x = DIRX;
	(ball->dir).y = DIRY;
	(ball->pos).x = POSX;
	(ball->pos).y = POSY;
	
	mvwprintw(game_win,POSY,POSX,"o"); //draws new ball
}

/*Moves ball from previous position to the next*/
void move_ball(WINDOW *game_win, WINDOW *win, ball_t *ball){

	mvwprintw(game_win,(ball->pos).y,(ball->pos).x," "); //clears previous position
	mvwprintw(win,0,0," ");
	
	(ball->pos).x = (ball->pos).x + (ball->dir).x; //computes new position
	(ball->pos).y = (ball->pos).y + (ball->dir).y;
	
	if(test_collision(ball)){ //detects collisions with the walls
		(ball->pos).x = (ball->pos).x + (ball->dir).x; //computes new position
		(ball->pos).y = (ball->pos).y + (ball->dir).y;
	}
	
	mvwprintw(game_win,(ball->pos).y,(ball->pos).x,"o"); //draws new ball in the given position
	mvwprintw(win,0,0," ");
	
}

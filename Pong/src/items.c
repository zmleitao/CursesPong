#include "items.h"
#include "window.h"

#define BUMPER_SIZE 3
#define BUMP_CH "|"

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

/*Tests collision with each wall; In case of collision, 1 is returned*/
int test_collision(ball_t *ball){
 int i,test_x;
	
	if((ball->pos).x == 0 && (ball->pos).y == 0){
		(ball->dir).x = (-1)*(ball->dir).x; //inverts direction
		(ball->dir).y = (-1)*(ball->dir).y;
		return 1;
	}
	else if((ball->pos).x == 0 && (ball->pos).y != 0){
		(ball->dir).x = (-1)*(ball->dir).x; //inverts x direction
		return 1;
	}
	else if((ball->pos).x != 0 && (ball->pos).y == 0){
				(ball->dir).y = (-1)*(ball->dir).y; //inverts y direction
				return 1;
	}
	else if((ball->pos).x == win_width -1 && (ball->pos).y == 0){
		(ball->dir).x = (-1)*(ball->dir).x; //inverts direction
		(ball->dir).y = (-1)*(ball->dir).y;
		return 1;
	}
	else if((ball->pos).x == win_width -1 && (ball->pos).y != 0){
		(ball->dir).x = (-1)*(ball->dir).x; //inverts x direction
		return 1;
	}
	else if((ball->pos).x == win_width -1 && (ball->pos).y == win_height -1){
		(ball->dir).x = (-1)*(ball->dir).x; //inverts direction
		(ball->dir).y = (-1)*(ball->dir).y;
		return 1;
	}
	else if((ball->pos).x != 0 && (ball->pos).y == win_height -1){
		(ball->dir).y = (-1)*(ball->dir).y; //inverts y direction
		return 1;
	}
	else if((ball->pos).x == 0 && (ball->pos).y == win_height -1){
		(ball->dir).x = (-1)*(ball->dir).x; //inverts direction
		(ball->dir).y = (-1)*(ball->dir).y;
		return 1;
	}
	else{

		for(i=0;i<2;i++){
			if(i==0)//testing bumper 1
				test_x = win_width-2;
			else //testing bumper 2
				test_x = 1;
		
				if((ball->pos).x == test_x){ //possible hit in bumper i
							pthread_mutex_lock(&mux_bumper[i]); //stops bumper position from being modified while it is read
							if((ball->pos).y == bumpers_y[i]){ //upper corner hit
								(ball->dir).x = (-1)*(ball->dir).x; //inverts direction
						
								if((ball->dir).y == 0)
									(ball->dir).y = -1; //deflects upwards
								else
									(ball->dir).y = (-1)*(ball->dir).y;
							
								pthread_mutex_unlock(&mux_bumper[i]);
								return 1;
							}
							if((ball->pos).y == bumpers_y[i] + BUMPER_SIZE -1){//lower corner hit
								(ball->dir).x = (-1)*(ball->dir).x; //inverts direction
						
								if((ball->dir).y == 0)
									(ball->dir).y = 1; //deflects downwards
								else
									(ball->dir).y = (-1)*(ball->dir).y;
							
								pthread_mutex_unlock(&mux_bumper[i]);
								return 1;
							}
							if((ball->pos).y == bumpers_y[i] + 1){//center hit
								(ball->dir).x = (-1)*(ball->dir).x; //inverts direction
								(ball->dir).y = 0;  //ball will move horizontally
						
								pthread_mutex_unlock(&mux_bumper[i]);
								return 1;
							}
							pthread_mutex_unlock(&mux_bumper[i]);
			}
		}
	}
 return 0; //no hit found
}



/*draws a bumper in the given position*/
void draw_bumper(WINDOW *bump_window, int starty, int startx){
	int i;
	
	for(i=0;i<BUMPER_SIZE;i++){
		mvwprintw(bump_window,starty + i,startx,BUMP_CH);
	}
	WinRefresh(bump_window);
}

/*moves the bumper to the specified position*/
void move_bumper(WINDOW *bump_window, int bumper_num, int dir){
	int i;
	
	pthread_mutex_lock(&mux_bumper[bumper_num]);
	if(dir){
		//bumper is moving up
		if(bumpers_y[bumper_num] != 0){ //window limit reached 
			mvwprintw(bump_window,bumpers_y[bumper_num] + BUMPER_SIZE -1,0," "); //erases trailing character
			mvwprintw(bump_window,bumpers_y[bumper_num] - 1,0,BUMP_CH); //prints character in the new position
			bumpers_y[bumper_num]--; //updates starting position
		}
	}
	else{
		//bumper is moving down
		if(bumpers_y[bumper_num] != win_height - 2 - BUMPER_SIZE){ 
			mvwprintw(bump_window,bumpers_y[bumper_num],0," "); //erases trailing character
			mvwprintw(bump_window,bumpers_y[bumper_num] + BUMPER_SIZE,0,BUMP_CH); //prints character in the new position
			bumpers_y[bumper_num]++;
		}
	}
	pthread_mutex_unlock(&mux_bumper[bumper_num]);
	WinRefresh(bump_window);
}

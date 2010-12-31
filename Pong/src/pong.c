#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <signal.h>
#include "window.h"
#include "items.h"
#define SPEED 80000
#define BUMPER_SIZE 3
#define BUMP_CH "|"


int bumpers_y[2]; //current positions of each bumper

int end = 0;




/*Tests collision with each wall; In case of collision, 1 is returned*/
int test_collision(ball_t *ball){
	
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
	return 0;
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
	

	WinRefresh(bump_window);
}

void *player(void *arg){
	int player_num;
	WINDOW *bumper_win;
	int x_pos;
	int ch;
	
	cbreak(); //line buffering disabled
	keypad(stdscr, TRUE); //enables F_keys
	noecho(); //disables echoing of characters
	
	
	player_num = *(int*)arg; //retrieves player number (1 or 2);
	
	if(player_num == 1){
		x_pos = win_x + win_width -2; //x coordinate
		bumper_win = create_newwin(win_height-2,1,win_y+1,x_pos,FALSE); //creates bumper window for player 1
		bumpers_y[0] = win_height/2; //initial bumper position
	}
	else if(player_num == 2){
				x_pos = win_x + 1; //x coordinate
				bumper_win = create_newwin(win_height-2,1,win_y+1,win_x,FALSE); //creates bumper window for player 2
				bumpers_y[1] = win_height/2;
	}
	
	draw_bumper(bumper_win,win_height/2,0); //draws bumper in the middle of the game window
	
	while(1){
		ch = getch();
		
		if(ch == 'q')//exit game
			break;
		
		if(player_num == 1){
			switch(ch){
				case KEY_UP:
					move_bumper(bumper_win,0,1); //moves bumper up
					break;
				case KEY_DOWN:
					move_bumper(bumper_win,0,0); //moves bumper down
					break;	
			}
		}
		if(player_num == 2)
		{
			switch(ch)
			{
				case 'W':
					move_bumper(bumper_win,1,--bumpers_y[1]); //moves bumper up
					break;
				case 'S':
					move_bumper(bumper_win,1,++bumpers_y[1]); //moves bumper down
					break;	
			}
		}
	}
	
	delwin(bumper_win); //deallocates window
	raise(SIGUSR1);
	
}

void close_game(){
	end = 1; //terminates game
}

int main(int argc, char **argv){

	WINDOW  *game_win;
	int player_num = 1;
	ball_t ball;
	unsigned int t_inter;
	pthread_t tid;



	
	WindowInit();
	
	refresh();// refresh window
	getch();

	game_win = GameWindowInit();
	
	mvwprintw(win,LINES-1,0,"Press 'q' to Exit      "); //changes bottom message
	
	new_ball(&ball,SPEED,game_win); //creates new ball
	
	WinRefresh(game_win);
	
	signal(SIGUSR1,close_game);
	
	/*creates player thread*/
	if(pthread_create(&tid,NULL,player,(void *)&player_num) != 0){
		fprintf(stderr,"Thread initialization failed\n");
		endwin();
		exit(-1);
	}
	
	while(!end){
		
		move_ball(game_win,win,&ball);
		WinRefresh(game_win);
		usleep(ball.speed);
	}
	

	endwin(); //end ncurses
	
	return 0;
}

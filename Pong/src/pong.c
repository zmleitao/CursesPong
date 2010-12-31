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



int end = 0;


pthread_mutex_t mux_char = PTHREAD_MUTEX_INITIALIZER;


void *player(void *arg){
	int player_num;
	WINDOW *bumper_win;
	int x_pos;
	int ch;
	
	cbreak(); //line buffering disabled
	keypad(stdscr, TRUE); //enables F_keys
	noecho(); //disables echoing of characters
	
	
	player_num = *(int*)arg; //retrieves player number (0 or 1);

	if(player_num == 0){
		x_pos = win_x + win_width -2; //x coordinate
		bumper_win = create_newwin(win_height-2,1,win_y+1,x_pos,FALSE); //creates bumper window for player 1
		bumpers_y[0] = win_height/2; //initial bumper position
	}
	else if(player_num == 1){
				x_pos = win_x + 1; //x coordinate
				bumper_win = create_newwin(win_height-2,1,win_y+1,win_x+1,FALSE); //creates bumper window for player 2
				bumpers_y[1] = win_height/2;
	}
	
	draw_bumper(bumper_win,win_height/2,0); //draws bumper in the middle of the game window
	

	while(1){
	//	printf("Player %d",player_num);
		pthread_mutex_lock(&mux_char);
		ch = getch();
		if(ch == 'q')//exit game
			break;
		
		if(player_num == 0){
			switch(ch){
				case KEY_UP:
					move_bumper(bumper_win,0,1); //moves bumper up
					break;
				case KEY_DOWN:
					move_bumper(bumper_win,0,0); //moves bumper down
					break;	
			}
			pthread_mutex_unlock(&mux_char);
		}
		else{
			switch(ch){
				case 'w':
					move_bumper(bumper_win,1,1); //moves bumper up
					break;
				case 's':
					move_bumper(bumper_win,1,0); //moves bumper down
					break;	
			}
			pthread_mutex_unlock(&mux_char);
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
	int *player_num ;
	int i;
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
	
	for(i=0;i<2; i++){ //creates player threads
		pthread_mutex_init(&mux_bumper[i],NULL); //initializes mutex
	
		player_num = (int*)malloc(sizeof(int));
		if(player_num == NULL)
			exit(-1);
			
		*player_num = i;
		if(pthread_create(&tid,NULL,player,(void *)player_num) != 0){
			fprintf(stderr,"Thread initialization failed\n");
			endwin();
			exit(-1);
		}
	}
	
	
	while(!end){
		
		move_ball(game_win,win,&ball);
		WinRefresh(game_win);
		usleep(ball.speed);
	}
	
	

	
	endwin(); //end ncurses
	
	return 0;
}

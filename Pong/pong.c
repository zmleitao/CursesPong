#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>

#define SPEED 80000
#define BUMPER_SIZE 3
#define BUMP_CH "|"

/*zml was here*/
char *string[5] = {".---.  .--. .-..-. .--.", ": .; :: ,. :: `: :: .--'", ":  _.': :: :: .` :: : _ ", ": :   : :; :: :. :: :; :", ":_;   `.__.':_;:_;`.__.'"};

int win_height,win_width,win_y,win_x; //size of game window and starting coordinates

int bumpers_y[2]; //current positions of each bumper

int end = 0;

typedef struct { int x,y;}position;

typedef struct {
	unsigned int speed; //ball speed corresponds to the time interval between consecutive movements
	position dir; //direction in which the ball is moving
	position pos;
}ball_t;

/*Prints a string centered on the coordinates given*/
void print_middle(WINDOW *win,int starty,int startx,char *string){
	float temp;
	
	if(startx != 0){
		temp = startx - (strlen(string)/2);
		startx = (int)temp;
	}
	
	mvwprintw(win,starty,startx,"%s",string);
}

/*Creates new window with or without borders*/
WINDOW *create_newwin(int height, int width, int starty, int startx, int border){
	WINDOW *local_win;
	
	local_win = newwin(height,width,starty,startx);
	if(border)
		box(local_win,0,0);
		
	wrefresh(local_win);
	
	return local_win;
}

/*Creates new ball*/
void new_ball(ball_t *ball, unsigned int speed, position dir, position pos){
	ball->speed = speed;
	(ball->dir).x = dir.x;
	(ball->dir).y = dir.y;
	(ball->pos).x = pos.x;
	(ball->pos).y = pos.y;
}

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

/*draws a bumper in the given position*/
void draw_bumper(WINDOW *bump_window, int starty, int startx){
	int i;
	
	for(i=0;i<BUMPER_SIZE;i++){
		mvwprintw(bump_window,starty + i,startx,BUMP_CH);
	}
	wrefresh(bump_window);
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
	wrefresh(bump_window);
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
		if(player_num == 2){
			switch(ch){
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
	int x,y;
	WINDOW *win, *game_win;
	float temp;
	int i,player_num = 1;
	position ball_pos,ball_dir;
	ball_t ball;
	unsigned int t_inter;
	pthread_t tid;
	
	win = initscr(); //start ncurses
	cbreak(); //line buffering disabled
	
	if(has_colors() == FALSE){
		endwin();
		printf("Your terminal does not support colors =<\n");
		exit(0);
	}
	start_color(); //start colors
	init_pair(1,COLOR_RED,COLOR_BLACK); //RED and BLACK color pair
	init_pair(2,COLOR_WHITE,COLOR_BLACK); //WHITE and BLACK color pair
	
	attron(COLOR_PAIR(1)); //turns on color pair 1
	
	x = COLS/2; //calculate center
	y = LINES/2 - 5;
	
	attron(A_BOLD | A_BLINK); //turns on BOLD and BLINKING
	for(i = 0;i<5;i++){
		print_middle(win,y,x,string[i]);
		y++;
	}
	attroff(A_BOLD | A_BLINK);
	mvprintw(LINES-1,0,"Brought to you by: smp");
	
	attron(COLOR_PAIR(2)); //turns on color pair 2
	y = y+4;
	print_middle(win,y,x,"Press any key to continue...");
	
	refresh();// refresh window
	getch();
	
	temp = LINES*0.8; //window will take 80% of the terminal size
	win_height = floor(temp);
	temp = COLS*0.8;
	win_width = floor(temp);
	
	win_x = ceil((COLS - win_width)/2);
	win_y = ceil((LINES - win_height)/2);
	
	
	game_win = create_newwin(win_height,win_width,win_y,win_x,TRUE); //creates game window
	
	mvwprintw(win,LINES-1,0,"Press 'q' to Exit      "); //changes bottom message
	
	ball_pos.x = 10; //initial position
	ball_pos.y = 15;
	ball_dir.x = -1; //initial direction
	ball_dir.y = -1;
	
	new_ball(&ball,SPEED,ball_dir,ball_pos); //creates new ball
	
	mvwprintw(game_win,ball_pos.y,ball_pos.x,"o"); //draws new ball
	wrefresh(game_win);
	
	signal(SIGUSR1,close_game);
	
	/*creates player thread*/
	if(pthread_create(&tid,NULL,player,(void *)&player_num) != 0){
		fprintf(stderr,"Thread initialization failed\n");
		endwin();
		exit(-1);
	}
	
	while(!end){
		move_ball(game_win,win,&ball);
		wrefresh(game_win);
		wrefresh(win);
		usleep(ball.speed);
	}
	
	
	endwin(); //end ncurses


	return 0;
}

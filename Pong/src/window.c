#include "window.h"

/*zml was here*/
char *string[5] = {".---.  .--. .-..-. .--.", ": .; :: ,. :: `: :: .--'", ":  _.': :: :: .` :: : _ ", ": :   : :; :: :. :: :; :", ":_;   `.__.':_;:_;`.__.'"};

/*Prints a string centered on the coordinates given*/
void print_middle(WINDOW *win,int starty,int startx,char *s){
	float temp;
	
	if(startx != 0){
		temp = startx - (strlen(s)/2);
		startx = (int)temp;
	}
	
	mvwprintw(win,starty,startx,"%s",s);
}

WINDOW * WindowInit(){
	
	WINDOW *win;
	int x,y,i;
	
	win = initscr(); //start ncurses
	cbreak(); //line buffering disabled

	if(has_colors() == FALSE)
	{
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
	for(i = 0;i<5;i++)
	{
	 print_middle(win,y,x,string[i]);
	 y++;
	}

	attroff(A_BOLD | A_BLINK);
	mvprintw(LINES-1,0,"Brought to you by: smp");

	attron(COLOR_PAIR(2)); //turns on color pair 2
	y = y+4;
	print_middle(win,y,x,"Press any key to continue...");
	
	return win;
}
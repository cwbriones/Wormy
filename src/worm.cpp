/*--------------------------------------------------------------------
 *  Wormy - A C++ Clone of Snake for UNIX Consoles
 *
 *  Copyright (c) 2012 Christian Briones <cwbriones@gmail.com>
 *  Version 1.1;
 *
 *  Compilation: make
 *--------------------------------------------------------------------*/

#include <cstdlib>
#include <ncurses.h>
#include <vector>
#include "worm.h"

using namespace std;

/* TO DO LIST
add a menu
add high scores at the end using fstream
fix the issue where the worm enters the wall during a collision
*/

//Fix apple behavior...look at cplusplusguys implementation

const int WORM_INIT = 5;

/*-----------------------------------------------------------------------------
 *  msleep: pauses execution for the given value in milliseconds, only created
 *  because usleep became cumbersome
 *-----------------------------------------------------------------------------*/
void msleep(int mSecs)
{
	usleep(mSecs*1000);
}


/*-----------------------------------------------------------------------------
 * addch_spec: Like addch, only draws with the given color pair and bold on/off 
 *-----------------------------------------------------------------------------*/
void addch_spec(char sym, int colorPair, bool bold)
{
	attron(COLOR_PAIR(colorPair));
	if(bold)
	{
		attron(A_BOLD);
	}
	addch(sym);
	if(bold)
	{
		attroff(A_BOLD);
	}
	attroff(COLOR_PAIR(colorPair));
}

itemPos::itemPos()
{
	x = 0;
	y = 0;
	return;
}

itemPos::itemPos(int xPos, int yPos)
{
	x = xPos;
	y = yPos;
	return;
}

Worm::Worm()
{
	srand(static_cast<unsigned int>(time(0))); //Seed the number generator
	
	//Our game symbols
	appSym = '@';
	wormSym = '0';
	wallSym = ' '; //Rectangle
	
	//ncurses initialization code from thecplusplusguy
	initscr(); //Initializes screen
	
	start_color();
	
	init_pair(WORM_PAIR, COLOR_GREEN, COLOR_GREEN);
	init_pair(APPLE_PAIR, COLOR_RED, COLOR_RED);
	init_pair(WALL_PAIR, COLOR_BLUE, COLOR_BLUE);
	init_pair(BLANK_PAIR, COLOR_BLACK, COLOR_BLACK);
	
	nodelay(stdscr, true); // the getch will not wait until user press key and preserve last value of the key
	keypad(stdscr, true); //Enables use of arrow keys among other things
	noecho();	//Disables echo to terminal
	curs_set(0); // Keeps the cursor from being displayed
	getmaxyx(stdscr, maxHeight, maxWidth); //Get window dimenions
	
	//initialize length 6 worm
	for(int i = 0; i < WORM_INIT; i++)
	{
		wBody.push_back(itemPos(40+i,10)); 
	}
	
	points = 0;
	delT = 150;	//Time in milliseconds between drawing the screen
    speedChange = 10;
	gotApple = false;
	gamePaused = false;
	direction = 'L';
	
	// upper horizontal

	for(int i=0; i< maxWidth-1; i++)
	{
		move(0,i);
		addch_spec(' ', WALL_PAIR, false);
	}
	// bottom line border
	for(int i=0; i< maxWidth-1; i++)
	{
		move(maxHeight-2,i);
		addch_spec(' ', WALL_PAIR, false);
	}
	// left border
	for(int i=0; i< maxHeight-1; i++)
	{
		move(i,0);
		addch_spec(' ', WALL_PAIR, false);
	}
	//right border
	for(int i=0; i< maxHeight-1; i++)
	{
		move(i,maxWidth-1);
		addch_spec(' ', WALL_PAIR, false);
	}
	
	for(int i = 1; i < maxHeight - 2; i++)
	{
		for(int j = 1; j < maxWidth - 1; j++)
		{
			move(i,j);
			addch_spec(' ', BLANK_PAIR, false);
		}
	}
	
	//Draw the worm
	for(int i=0; i< wBody.size(); i++)
	{
		move(wBody[i].y,wBody[i].x);
		addch_spec(' ', WORM_PAIR, false);
	}
	
	
	//Displays Points on Bottom Line
	move(maxHeight-1,0);
	printw("Score: %d", points);
	
	//Now Add the Apple
	AddApple();
	refresh();
}

Worm::~Worm()
{
	nodelay(stdscr, false);
		getch(); //Waits for user to press a key before exit
	endwin();
}


/*-----------------------------------------------------------------------------
 *  AddApple: Places the apple at a random location and then draws it to the
 *  window
 *-----------------------------------------------------------------------------*/
void Worm::AddApple()
{
	bool cont = false;
	while(!cont)
	{
		applePos.x = rand()%(maxWidth - 2) + 1;
		applePos.y = rand()%(maxHeight - 3) + 1;
		
		cont = true;
		
		for(int i = 0; i < wBody.size(); i++)
		{
			if(applePos.x == wBody[i].x && applePos.y == wBody[i].y)
				cont = false;
		}
	}
	move(applePos.y, applePos.x);
	addch_spec(' ', APPLE_PAIR, false);
}


/*-----------------------------------------------------------------------------
 *  CheckCollision: Checks for any position based logic for the worm, including
 *  wall collsions, self-collisions, and collisions with the apple
 *-----------------------------------------------------------------------------*/
bool Worm::CheckCollision()
{
	//Why -3 for y maxHeight? Wall Collisions
	if(wBody[0].y == 0 || wBody[0].y == maxHeight - 2 || 
	   wBody[0].x == maxWidth-1 || wBody[0].x == 0)
	{
		return true;
	}
	//Self-Collision
    for(int i = 4; i < wBody.size(); i++)
    {
        if(wBody[i].x == wBody[0].x && wBody[i].y == wBody[0].y)
            return true;
    }
	//Has gotten the food
	if(wBody[0].x == applePos.x && wBody[0].y == applePos.y)
	{
		gotApple = true;
		
		AddApple();
		points += 10;
		move(maxHeight-1,7);
		printw("%d", points);
		
		//Speed up the game here by changing delT
        if(delT - speedChange >= 10 && points%70 == 0)
        {
            delT -= speedChange;
        }
	}
	return false;
}


/*-----------------------------------------------------------------------------
 *  MoveWorm: Takes in keyboard input and moves the worm accordingly, also
 *  moves the worm onscreen by drawing its new position to the window
 *-----------------------------------------------------------------------------*/
void Worm::MoveWorm()
{
	int tmp = getch();
	
	switch(tmp)	//Check for user input
	{
		case KEY_LEFT:
			if(direction != 'R')
				direction = 'L';
			break;
		case KEY_RIGHT:
			if(direction != 'L')
				direction = 'R';
			break;
		case KEY_UP:
			if(direction != 'D')
				direction = 'U';
			break;
		case KEY_DOWN:
			if(direction != 'U')
				direction = 'D';
			break;
		case (int)'q':
			direction = 'Q';
			break;
		case (int)'p':
			gamePaused = true;
	}
	
	//remove the end as the worm moves forward
	if(!gotApple)
	{
		move(wBody[wBody.size() - 1].y, wBody[wBody.size() - 1].x);
		addch_spec(' ', BLANK_PAIR, false);
		wBody.pop_back();
	} else {
		gotApple = false;
	}
	switch(direction)
	{
		case 'L':
			wBody.insert(wBody.begin(),itemPos(wBody[0].x - 1,wBody[0].y));
			break;
		case 'R':
			wBody.insert(wBody.begin(),itemPos(wBody[0].x + 1,wBody[0].y));
			break;
		case 'U':
			wBody.insert(wBody.begin(),itemPos(wBody[0].x,wBody[0].y - 1));
			break;
		case 'D':
			wBody.insert(wBody.begin(),itemPos(wBody[0].x,wBody[0].y + 1));
			break;
	}
	
	move(wBody[0].y, wBody[0].x);
	addch_spec(' ', WORM_PAIR, true);
}


/*-----------------------------------------------------------------------------
 *  FlashWorm: Alternates between drawing the worm normally and red
 *  to visually inform the user of a collision
 *-----------------------------------------------------------------------------*/
void Worm::FlashWorm()
{
	int flashes = 3;
	int curColor = 0;
	
	for(int j = 0; j < flashes*2; j++)
	{
		if(j%2 == 0) {
			curColor = APPLE_PAIR;
		} else {
			curColor = WORM_PAIR;
		}
		for(int i = 0; i < wBody.size(); i++)
		{
			move(wBody[i].y, wBody[i].x);
			addch_spec(' ', curColor, false);
		}
		refresh();
		msleep(400);
	}
}

void Worm::StartGame()
{
	while(true)
	{
		MoveWorm();
		refresh();
		if(CheckCollision())
		{
			move(12, 36);
			printw("Game Over");
			move(14, 30);
			printw("Press Any Key to Exit");
			FlashWorm();
			break;
		}
		
		if(direction == 'Q')
		{
			move(14, 30);
			printw("Press Any Key to Exit");
			break;
		} 
		else if(gamePaused) //Player pauses
		{
			while(gamePaused)
			{
				move(12, 36);
				printw("Game Paused");
				msleep(400);
				//Check for return
				if(getch() == 'p')
				{
					gamePaused = false;
					move(12, 36);
					printw("           ");
					//On return, remove message and redraw worm
					//Just in case it was covered by the message
					for(int i = 0; i < wBody.size(); i++)
					{
						move(wBody[i].y, wBody[i].x);
						addch_spec(' ', WORM_PAIR, false);
					}
				}
			}
		}
		if(direction == 'U' || direction == 'D')
		{
			msleep(delT);
		} 
		else if (direction == 'L' || direction == 'R')
		{
			msleep(0.75*delT);
		}
	}
	
}

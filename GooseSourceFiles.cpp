/*
 Nicholas Borisenko
 Justin Wang
*/
//------------------- START OF gooseEscapeMain.cpp ----------------------//

#include <BearLibTerminal.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
using namespace std;
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"

//set up the console.   Don't modify this line!
Console out;

int main()
{
	//Set up the window.  Don't edit these two lines
    terminal_open();
  	terminal_set(SETUP_MESSAGE);

/*
    The code below provides a skeleton of the game play.  You will need to
    write code for setting up the game board, and playing the game itself.
    You can modify the code given as needed.
    
    Call the functions that you have written in the game play file, and that
    you have added to the Actor class.
*/
 	
    //make the player

	
    // Declare the array that will hold the game board "map"
    int const HEIGHT = NUM_BOARD_Y;
    int const WIDTH = NUM_BOARD_X;
    int gameMap[HEIGHT][WIDTH]={0};
    
/*
    Initiallize locations in the game board to have game features.  What if you
    have man things to add to the game board?  Should you use a loop?  Does it
    make sense to store this information in a file?  Should this code be a
    function as well?
*/

	for(int i = 0 ; i < HEIGHT ; i++)
	{
		gameMap[i][0] = SHALL_NOT_PASS;
		gameMap[i][WIDTH-1] = SHALL_NOT_PASS;
	}
		for(int i = 0 ; i < WIDTH ; i++)
	{
		gameMap[0][i] = SHALL_NOT_PASS;
		gameMap[HEIGHT-1][i] = SHALL_NOT_PASS;
	}
	
	
	for(int i = 20 ; i < WIDTH-20 ; i++)
	{
		gameMap[5][i] = SHALL_NOT_PASS;
	}
	

	int safezoneX = 25;
	int safezoneY = 15;
	gameMap[safezoneY][safezoneX] = WINNER;

	int playerStartX = randomPosition(70);
	int playerStartY = randomPosition(20);
	
	int gooseStartX = 70;
	int gooseStartY = 15;
	
	int snowballX = 25;
	int snowballY = 10;
	
	while(gameMap[playerStartY][playerStartX] !=0 || (playerStartX == gooseStartX && playerStartY == gooseStartY) ||
			abs(playerStartX - gooseStartX) < 5 || abs(playerStartY - gooseStartY )<5)
	{
		playerStartX = randomPosition(70);
		playerStartY = randomPosition(20);
	}
	
    Actor player(PLAYER_CHAR, playerStartX, playerStartY);
	
	Actor snowball(SNOWBALL_CHAR, snowballX, snowballY);
	
	Actor goose(GOOSE_CHAR, gooseStartX,gooseStartY);
	
  	printBoard(gameMap);
  	
  	
	// Printing the instructions
    out.writeLine("Escape the Goose! " + goose.get_location_string() );
	out.writeLine("Use the arrow keys to move");
	out.writeLine("If the goose catches you, you lose!");
	out.writeLine("Be careful! Sometimes the goose can jump through walls!");

/*
    This is the main game loop.  It continues to let the player give input
    as long as they do not press escape or close, they are not captured by
    the goose, and they didn't reach the win tile
*/
/*
    All key presses start with "TK_" then the character.  So "TK_A" is the "a"
    key being pressed.
*/
    int keyEntered = TK_A;  // can be any valid value that is not ESCAPE or CLOSE
	int gooseFrozenMoves = 0;
    while(keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE 
                    && !captured(player,goose) && !isGameWon(player, gameMap))///<< ADD SAFEZONE
	{
	    // get player key press
	    
	    keyEntered = terminal_read();
	    
		if(captured(player,snowball))
		{
			gooseFrozenMoves = 5;
		}
		
		
        if (keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE)
        {
            // move the player, you can modify this function
    	
		movePlayer(keyEntered,player, gameMap);
		isGooseFrozen(gooseFrozenMoves, player, goose, gameMap);

            // call the goose's chase function
            // call other functions to do stuff?	    
        }
        
        gooseFrozenMoves -= 1;
  	}

    if (keyEntered != TK_CLOSE)
    {
    	
      	//once we're out of the loop, the game is over
        out.writeLine("Game has ended");
        out.writeLine(outputGameResult(player, goose, gameMap));


    
        // output why:  did the goose get us, or did we get to the win location?
	
    	// Wait until user closes the window
        while (terminal_read() != TK_CLOSE);
    }

	//game is done, close it  
    terminal_close();
}

//------------------- START OF gooseEscapeGamePlay.cpp ----------------------//
#include <iostream>
#include <cmath>
using namespace std;
#include <BearLibTerminal.h>
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <cstdlib>

extern Console out;
/*
With graphics, screens are given an x,y coordinate system with the origin
in the upper left corner.  So it means the coordinate axes are:
---------------->   x direction
|
|
|
|
|
V

y direction 
*/
	
/*
    Print the game world
    
    The functions should draw characters to present features of the game
    board, e.g. win location, obstacles, power ups
*/

// print the game board function
void printEntity(int x_location_on_board, int y_location_on_board,int symbol)
{
    terminal_put(x_location_on_board,y_location_on_board,symbol);
}

/*
    Do something when the goose captures the player
    
    If you want to attack or something else, this is the function you 
    need to change.  For example, maybe the two touch each other and
    then fight.  You could add a health to the Actor class that is
    updated.  Run, use weapons, it's up to you!
*/

bool captured(Actor const & player, Actor const & goose)
{
    return (player.get_x() == goose.get_x() 
         && player.get_y() == goose.get_y());
}

/*
    Move the player to a new location based on the user input.  You may want
    to modify this if there are extra controls you want to add.
    
    All key presses start with "TK_" then the character.  So "TK_A" is the a
    key being pressed.
    
    A look-up table might be useful.
    You could decide to learn about switch statements and use them here.
*/

void movePlayer(int key, Actor & player,int gameBoard[NUM_SCREEN_Y][NUM_SCREEN_X])
{
    int yMove = 0, xMove = 0;
    if (key == TK_UP && gameBoard[player.get_y()-1][player.get_x()]!= SHALL_NOT_PASS )
        yMove = -1;
    else if (key == TK_DOWN && gameBoard[player.get_y()+1][player.get_x()]!= SHALL_NOT_PASS)
        yMove = 1;
    else if (key == TK_LEFT && gameBoard[player.get_y()][player.get_x()-1]!= SHALL_NOT_PASS)
        xMove = -1;
    else if (key == TK_RIGHT && gameBoard[player.get_y()][player.get_x()+1]!= SHALL_NOT_PASS)
        xMove = 1;
        
    if (player.can_move(xMove, yMove))
    {
        player.update_location(xMove, yMove);
    }
}

/*
    What other functions do you need to make the game work?  What can you
    add to the basic functionality to make it more fun to play?
*/
void moveGoose( Actor & player, Actor & goose, int gameBoard[NUM_SCREEN_Y][NUM_SCREEN_X])
{
	int xGooseMove = 0, yGooseMove = 0;
	
	if(player.get_x() < goose.get_x() && gameBoard[goose.get_y()][goose.get_x()-1]!= SHALL_NOT_PASS)
	{
		xGooseMove = -1;
	}
	if(player.get_x() > goose.get_x() && gameBoard[goose.get_y()][goose.get_x()+1]!= SHALL_NOT_PASS)
	{
		xGooseMove = 1;
	}
	if(player.get_y() < goose.get_y() && gameBoard[goose.get_y()-1][goose.get_x()]!= SHALL_NOT_PASS)
	{
		yGooseMove = -1;
	}
	if(player.get_y() > goose.get_y() && gameBoard[goose.get_y()+1][goose.get_x()]!= SHALL_NOT_PASS)
	{
		yGooseMove = 1;
	}

	if (goose.can_move(xGooseMove, yGooseMove))
    {
        goose.update_location(xGooseMove, yGooseMove);
    }
	
}

void printBoard(int gameBoard[NUM_SCREEN_Y][NUM_SCREEN_X])
{
	for(int yIndex = 0; yIndex < NUM_SCREEN_Y; yIndex++)
	{
		for(int xIndex = 0; xIndex < NUM_SCREEN_X; xIndex++)
		{
			if(gameBoard[yIndex][xIndex] == SHALL_NOT_PASS)
			{
				printEntity(xIndex, yIndex, WALL_CHAR);
			}
			if(gameBoard[yIndex][xIndex] == WINNER)
			{
				printEntity(xIndex, yIndex, WIN_CHAR);
			}
		
		}
	}
}

bool isGameWon(Actor & player, int gameBoard[NUM_SCREEN_Y][NUM_SCREEN_X])
{
	if( gameBoard[player.get_y()][player.get_x()]==WINNER)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int randomPosition(int max)
{
	srand(time(0));
	int result =  1 + rand()%max;
	return result;
}


void isGooseFrozen(int movesRemaining, Actor & player, Actor & goose, int gameBoard[NUM_SCREEN_Y][NUM_SCREEN_X])
{
	if(movesRemaining<1)
	{
		moveGoose(player, goose, gameBoard);
	}
}

string outputGameResult(Actor & player, Actor & goose, int gameBoard[NUM_SCREEN_Y][NUM_SCREEN_X])
{
	string gameMessage = "";
	
	if(player.sameActorPosition(goose))
	{
		gameMessage = "YOU HAVE LOST BECAUSE THE GOOSE CAUGHT YOU!!!";
	}
	else if(isGameWon(player,gameBoard))
	{
		gameMessage = "YOU ARE SAFE, SAFEZONE REACHED!!!";
	}
	else
	{
	
		return gameMessage;
	}	
}

//------------------- START OF gooseEscapeGamePlay.hpp ----------------------//

#ifndef GOOSE_ESCAPE_GAMEPLAY
#define GOOSE_ESCAPE_GAMEPLAY
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"

/*This file is all about the game world.  You will modify this to add
    constants and function prototypes.  Modify gooseGamePlay.cpp to
	actually add functionality.
*/

/*
    Declare constants to indicate various game world features in the board
    array.  Modify them to fit what you would like to do in the game.  You can
    change the type if you choose to store your game board as something other
    than intengers.
*/	
// Going further:  Learn how to use an enum for these values
const int EMPTY = 0;
const int SHALL_NOT_PASS = 1;
const int WINNER = 2;
const int ACTOR_SPACE =3;

/*
    A few examples of characters both for actors and for the game board
    itself are shown.
*/	
//display characters
const int PLAYER_CHAR = int('@');
const int GOOSE_CHAR = int('G');
const int WALL_CHAR = int('o');
const int WIN_CHAR = int('%'); //% sign, a special character used in the ancient game "Rogue"
const int SNOWBALL_CHAR = int('S');

/*
    Game play function prototypes are give below.
*/

// print the game board function protype
void printEntity(int x_location_on_board, int y_location_on_board,int symbol);
/*
    Do something when the goose captures the player
    
    If you want to attack or something else, this is the function you 
    need to change.  For example, maybe the two touch each other and
    then fight.  You could add a health to the Actor class that is
    updated.  Run, use weapons, it's up to you!
*/
bool captured(Actor const & player, Actor const & goose);

/*
    Move the player to a new location based on the user input.  You may want
    to modify this if there are extra controls you want to add.
    
    All key presses start with "TK_" then the character.  So "TK_A" is the a
    key being pressed.
    
    A look-up table might be useful.
    You could decide to learn about switch statements and use them here.
*/

void movePlayer(int key, Actor & player, int gameBoard[NUM_SCREEN_Y][NUM_SCREEN_X]);

/*
    What other functions do you need to make the game work?  What can you
    add to the basic functionality to make it more fun to play?
*/

void moveGoose( Actor & player, Actor & goose, int gameBoard[NUM_SCREEN_Y][NUM_SCREEN_X]);


void printBoard(int gameBoard[NUM_SCREEN_Y][NUM_SCREEN_X]);


bool isGameWon(Actor & player, int gameBoard[NUM_SCREEN_Y][NUM_SCREEN_X]);

int randomPosition(int max);

void isGooseFrozen(int movesRemaining, Actor & player, Actor & goose, int gameBoard[NUM_SCREEN_Y][NUM_SCREEN_X]);

string outputGameResult(Actor & player, Actor & goose, int gameBoard[NUM_SCREEN_Y][NUM_SCREEN_X]);


#endif

//------------------- START OF gooseEscapeActors.hpp ----------------------//

#ifndef GOOSE_ESCAPE_ACTORS
#define GOOSE_ESCAPE_ACTORS
#include <cmath>
#include <BearLibTerminal.h>
#include "gooseEscapeUtil.hpp"

/*
    Modify this class to contain more characteristics of the "actor".  Add
    functions that will be useful for playing the game that are specific to
    the Actor.
    
    Feel free to add additional Classes to your program.
*/

/* 
    Going further:  Learn the other syntax for implementing a class that is
    more appropriate for working with multiple files, and improve the class code.
*/

class Actor
{
  private:
    int actorChar;      
    int location_x, location_y;

  public:
    Actor()
    {
        actorChar = int('A');
        location_x = MIN_SCREEN_X;
        location_y = MIN_SCREEN_Y;
        put_actor();
    }

    Actor(char initPlayerChar, int x0, int y0)
    {
        change_char(initPlayerChar);
        location_x = MIN_SCREEN_X;
        location_y = MIN_SCREEN_Y;
        update_location(x0,y0);
    }
    
    int get_x() const
    {
        return location_x;
    }
    
    int get_y() const
    {
        return location_y;
    }
    
    string get_location_string() const
    {
        char buffer[80];
        itoa(location_x,buffer,10);
        string formatted_location = "(" + string(buffer) + ",";
        itoa(location_y,buffer,10);
        formatted_location += string(buffer) + ")";
        return formatted_location;
    }
    
    void change_char(char new_actor_char)
    {
        actorChar = min(int('~'),max(int(new_actor_char),int(' ')));
    }

    bool can_move(int delta_x, int delta_y) const
    {
        int new_x = location_x + delta_x;
        int new_y = location_y + delta_y;

        return new_x >= MIN_BOARD_X && new_x <= MAX_BOARD_X
          && new_y >= MIN_BOARD_Y && new_y <= MAX_BOARD_Y;
    }

    void update_location(int delta_x, int delta_y)
    {
        if (can_move(delta_x, delta_y))
        {
            terminal_clear_area(location_x, location_y, 1, 1);
            location_x += delta_x;
            location_y += delta_y;
            put_actor();
        }
    }
    
    void put_actor() const
    {
        terminal_put(location_x, location_y, actorChar);
        terminal_refresh();
    }
    
    void remove_actor()
    {
    	terminal_clear_area(location_x, location_y, 1, 1);
	}
	
	bool sameActorPosition(Actor & otherActor)
	{
		if (location_x == otherActor.get_x() && location_y == otherActor.get_y()) 
		{
			return true;

		}
		else
		{
			return false;
		}
	}
	
	
    
    
    
    
};
#endif
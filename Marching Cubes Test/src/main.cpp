//#include <SDL2/SDL.h>
#define FMT_HEADER_ONLY

#include <fmt/core.h>
#include <iostream>
#include "../headers/game.h"

int main(int argc, char* argv[]) 
{
	
	
	initSDL();
	initGame();
	gameLoop();
	

	

	return 0;
}

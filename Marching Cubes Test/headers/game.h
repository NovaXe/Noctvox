#pragma once
#include <SDL2/SDL.h>


struct SDL_Deleter {
	void operator()(SDL_Window* p) const {
		SDL_DestroyWindow(p);
	}
	/*void operator()(SDL_GLContext*  p) const {
		SDL_GL_DeleteContext(p);
	}*/
};




using UWindow = std::unique_ptr<SDL_Window, SDL_Deleter>;





void initSDL();


bool initGL();

void gameLoop();

void processEvents();

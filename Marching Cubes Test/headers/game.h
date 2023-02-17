#pragma once
#include <SDL2/SDL.h>

namespace noctvox {

	class Game {
	private:
		// world
		// players
		// events

		int screen_width;
		int screen_height;

		bool is_running;
		bool is_paused;

		SDL_Window* gWindow;
		SDL_Renderer* gRenderer;




		//SDL_Event event_handler;
		bool initSDL();
		bool initWindow();
		void initCallbacks();

	public:
		Game();
		~Game();



		void startGameLoop();
		void logicUpdate();
		void render();



	};

}
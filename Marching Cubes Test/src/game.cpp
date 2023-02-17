#define FMT_HEADER_ONLY
#include <fmt/core.h>

#include "../headers/game.h"


namespace noctvox {


	Game::Game()
	{
		initSDL();

		screen_width = 800;
		screen_height = 600;

		initWindow();
		is_running = true;
		startGameLoop();

	}

	Game::~Game()
	{

	}

	bool Game::initSDL() {
		// Intialize SDL for video
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			fmt::print("SDL could not intialize! SDL Error: {}s\n", SDL_GetError());
			return false;
		}
	}


	bool Game::initWindow()
	{
		SDL_Window* gWindow = SDL_CreateWindow("Marching Cubes Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			fmt::print("Window could not be created! SDL Error: {}s\n", SDL_GetError());
			return false;
		}

		gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
		if (gRenderer == NULL)
		{
			fmt::print("Renderer could not be creasted! SDL Error: {}s\n", SDL_GetError());
			return false;
		}

		// INitialize renderer color
		SDL_SetRenderDrawColor(gRenderer, 0x81, 0x5c, 0xb2, 0xFF);

		return true;


	}


	void Game::initCallbacks()
	{

	}

	void Game::startGameLoop()
	{

		const double TICK_RATE = 1.0 / 60.0; // 60 FPS
		double nextTick = SDL_GetTicks() / 1000.0; // start time in seconds
		double accumulatedTime = 0.0;

		SDL_Event event;
		while (is_running) {
			double currentTime = SDL_GetTicks() / 1000.0;
			accumulatedTime += (currentTime - nextTick);
			nextTick = currentTime;

			int ticksThisFrame = 0;

			while (accumulatedTime >= TICK_RATE) {
				if (ticksThisFrame >= 4) { break; }
				ticksThisFrame++;

				logicUpdate();
				accumulatedTime -= TICK_RATE;
			}
			render();
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					is_running = false;
				}
			}
		}
	}

	void Game::logicUpdate()
	{

	}

	void Game::render()
	{
		SDL_RenderClear(gRenderer);
		SDL_RenderPresent(gRenderer);

	}
}
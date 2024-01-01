#include <stdio.h>

#include <SDL.h>

#include "log.h"
#include "game.h"

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 720


int main(int argc, char *argv[]) {
	LOG_DEBUG("Debugging is enabled. Hello World! The year is (now) %d", 2024);

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		LOG_ERROR("SDL_INIT error: %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Window *win = SDL_CreateWindow(
			"Tetris",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH, WINDOW_HEIGHT,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
	);
	if (win == NULL) {
		LOG_ERROR("SDL_CreateWindow error: %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(
			win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (renderer == NULL) {
		LOG_ERROR("SDL_CreateRenderer error: %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	start_game(renderer);

	SDL_DestroyRenderer(renderer);
	SDL_Quit();

    return 0;
}

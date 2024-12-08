#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void lerpPoint(int *x, int *y, int startX, int startY, int targetX, int targetY, float t) {
	*x = (1 - t) * startX + t * targetX;
	*y = (1 - t) * startY + t * targetY;
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	srand((unsigned int)time(NULL));

	SDL_Window *window = SDL_CreateWindow("Particle Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
										  SDL_WINDOW_OPENGL);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Rect p1 = {20, 20, 5, 5};
	int startX = 20, startY = 20;
	int targetX = rand() % 800, targetY = rand() % 600;
	float t = 0.0f;
	float speed = 0.01f;
	bool running = true;
	SDL_Event event;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
		}
		if (t < 1.0f) {
			t += speed;
			lerpPoint(&p1.x, &p1.y, startX, startY, targetX, targetY, t);
		}
		else {
			t = 0.0f;
			startX = targetX;
			startY = targetY;
			targetX = rand() % 800;
			targetY = rand() % 600;
		}

		// Rendering
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &p1);

		SDL_RenderPresent(renderer);

		SDL_Delay(10);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct tParticle
{
	int startX, startY, targetX, targetY;
	float t, speed;
	SDL_Rect p;
};

struct tParticle particleInit() {
	struct tParticle p1;
	p1.startX = 20;
	p1.startY = 20;
	p1.targetX = rand() % 800;
	p1.targetY = rand() % 600;
	p1.speed = 3.5f;
	p1.t = 0.0f;
	p1.p.h = 5;
	p1.p.w = 5;
	p1.p.x = p1.startX;
	p1.p.y = p1.startY;
	return p1;
}

void lerpPoint(int *x, int *y, int startX, int startY, int targetX, int targetY, float t) {
	*x = (int)((1 - t) * startX + t * targetX);
	*y = (int)((1 - t) * startY + t * targetY);
}

SDL_Rect movePoint(int *startX, int *startY, int *targetX, int *targetY, float *t, float speed) {
	float dx = *targetX - *startX;
	float dy = *targetY - *startY;
	float distance = sqrtf(dx * dx + dy * dy);

	SDL_Rect result = {.x = *startX, .y = *startY, .w = 5, .h = 5};

	if (distance > 0) {
		float moveDir = speed / distance;
		*t += moveDir;

		if (*t >= 1.0f) {
			*t = 1.0f;
		}

		lerpPoint(&result.x, &result.y, *startX, *startY, *targetX, *targetY, *t);
	}

	return result;
}

void checkRandomMove(struct tParticle *p) {
	if (p->t >= 1.0f) {
		p->startX = p->targetX;
		p->startY = p->targetY;
		p->targetX = rand() % 800;
		p->targetY = rand() % 600;
		p->t = 0.0f;
	}
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	srand((unsigned int)time(NULL));

	SDL_Window *window = SDL_CreateWindow("Particle Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
										  SDL_WINDOW_OPENGL);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	struct tParticle *p;
	int N = 100;
	bool running = true;
	SDL_Event event;

	// printf("Enter the number of particles:");
	// scanf("%d", &N);
	p = calloc(N, sizeof(struct tParticle));
	for (int i = 0; i < N; i++)
		p[i] = particleInit();

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
		}

		for (int i = 0; i < N; i++) {
			p[i].p = movePoint(&p[i].startX, &p[i].startY, &p[i].targetX, &p[i].targetY, &p[i].t, p[i].speed);
			checkRandomMove(&p[i]);
		}

		// Rendering
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		for (int i = 0; i < N; i++)
			SDL_RenderFillRect(renderer, &p[i].p);

		SDL_RenderPresent(renderer);

		SDL_Delay(10);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

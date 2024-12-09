#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

struct tParticle
{
	int startX, startY, targetX, targetY;
	float t, speed;
	SDL_Rect p;
};

void checkRandomMove(struct tParticle *p);

bool isOverlapping(struct tParticle *particles, int count, int x, int y) {
	for (int i = 0; i < count; i++) {
		if (particles[i].p.x == x && particles[i].p.y == y) {
			return true;
		}
	}
	return false;
}

struct tParticle particleInitUnique(struct tParticle *particles, int count) {
	struct tParticle p1;
	do {
		p1.startX = 20 + rand() % 20;
		p1.startY = 20 + rand() % 20;
	} while (isOverlapping(particles, count, p1.startX, p1.startY));

	p1.targetX = rand() % SCREEN_WIDTH;
	p1.targetY = rand() % SCREEN_HEIGHT;
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
	int v = speed;

	SDL_Rect result = {.x = *startX, .y = *startY, .w = 5, .h = 5};

	if (*startX > SCREEN_WIDTH || *startY > SCREEN_HEIGHT || *startX < 0 || *startY < 0)
		v = -v;

	if (distance > 0) {
		float moveDir = v / distance;
		*t += moveDir;

		if (*t >= 1.0f) {
			*t = 1.0f;
		}

		lerpPoint(&result.x, &result.y, *startX, *startY, *targetX, *targetY, *t);
	}

	return result;
}

bool rectsCollide(SDL_Rect *a, SDL_Rect *b) {
	return (a->x < b->x + b->w && a->x + a->w > b->x && a->y < b->y + b->h && a->y + a->h > b->y);
}

void resolveCollision(struct tParticle *a, struct tParticle *b) {
	float dx = b->p.x - a->p.x;
	float dy = b->p.y - a->p.y;

	float magnitude = sqrtf(dx * dx + dy * dy);
	if (magnitude == 0)
		magnitude = 1;
	dx /= magnitude;
	dy /= magnitude;

	int separation = 5;
	a->p.x -= (int)(dx * separation);
	a->p.y -= (int)(dy * separation);
	b->p.x += (int)(dx * separation);
	b->p.y += (int)(dy * separation);

	a->targetX = a->p.x - (int)(dx * 100);
	a->targetY = a->p.y - (int)(dy * 100);
	b->targetX = b->p.x + (int)(dx * 100);
	b->targetY = b->p.y + (int)(dy * 100);

	a->startX = a->p.x;
	a->startY = a->p.y;
	b->startX = b->p.x;
	b->startY = b->p.y;

	a->t = 0.0f;
	b->t = 0.0f;
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

void collideWall(struct tParticle *p) {
	if (p->p.x + p->p.w <= 0 || p->p.x - p->p.w >= SCREEN_WIDTH) {
		p->targetX = 2 * p->p.x - p->startX;
		p->startX = p->p.x;

		if (p->p.x <= 0)
			p->p.x = 1;
		if (p->p.x >= SCREEN_WIDTH)
			p->p.x = SCREEN_WIDTH - 1;
	}

	if (p->p.y + p->p.h <= 0 || p->p.y - p->p.h >= SCREEN_HEIGHT) {
		p->targetY = 2 * p->p.y - p->startY;
		p->startY = p->p.y;

		if (p->p.y <= 0)
			p->p.y = 1;
		if (p->p.y >= SCREEN_HEIGHT)
			p->p.y = SCREEN_HEIGHT - 1;
	}

	p->t = 0.0f;
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	srand((unsigned int)time(NULL));

	SDL_Window *window = SDL_CreateWindow("Particle Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
										  SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	struct tParticle *p;
	int N = 400;
	bool running = true;
	SDL_Event event;

	// printf("Enter the number of particles:");
	// scanf("%d", &N);
	p = calloc(N, sizeof(struct tParticle));
	for (int i = 0; i < N; i++) {
		p[i] = particleInitUnique(p, i);
	}

	/*p[0].startX = 20;
	p[0].startY = 200;
	p[0].targetX = 760;
	p[0].targetY = 200;

	p[1].startX = 760;
	p[1].startY = 200;
	p[1].targetX = 20;
	p[1].targetY = 200;*/

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
		}

		for (int i = 0; i < N; i++) {
			p[i].p = movePoint(&p[i].startX, &p[i].startY, &p[i].targetX, &p[i].targetY, &p[i].t, p[i].speed);
			// checkRandomMove(&p[i]);
			for (int j = i + 1; j < N; j++) {
				if (rectsCollide(&p[i].p, &p[j].p)) {
					printf("Collision detected between particle %d and %d\n", i, j);
					resolveCollision(&p[i], &p[j]);
				}
				//collideWall(&p[i]);
				if (p[i].p.x == p[i].targetX && p[i].p.y == p[i].targetY) {
					int tmp = p[i].startX;
					p[i].startX = p[i].targetX;
					p[i].targetX = tmp;
					tmp = p[i].startY;
					p[i].startY = p[i].targetY;
					p[i].targetY = tmp;
					p[i].t = 0.0f;
				}
			}
			// printf("Particle: %d, X: %d, Y: %d, StartX: %d\n", i, p[i].p.x, p[i].p.y, p[i].startX);
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

	free(p);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

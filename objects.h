#ifndef OBJECTS_H
#define OBJECTS_H

#include <SDL2/SDL.h>

struct Bullet {
	SDL_Rect position;
    int face;
    int player_id;
};

struct Player {
	SDL_Rect position;
	SDL_Texture *texture;
	int left, right, up, down;
};

void player_key_up (int key, struct Player* player);
void player_key_down (int key, struct Player* player);
void move_player (struct Player*);
void resolve_keyboard(SDL_Event e, struct Player* player);

#endif
#ifndef OBJECTS_H
#define OBJECTS_H

#include <SDL2/SDL.h>

struct Bullet {
	SDL_Rect position;
    int face;
    int player_id; 					//used for checking whether this bullet can kill a player
};

struct Player {
	SDL_Rect position;
	SDL_Texture *texture;
	int left, right, up, down;
	int kills, deaths;
	int face, shoot, reloading;
	int player_id;				//used for checking whether the player can be killed by a bullet
	bool pacman;
};

void player_key_up (int key, struct Player* player);
void player_key_down (int key, struct Player* player);
void set_player_pos(struct Player* player, float x, float y);
void resolve_keyboard(SDL_Event e, struct Player* player);

#endif
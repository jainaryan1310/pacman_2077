#include "objects.h"

#define STEP 5

void player_key_down(int key, struct Player* player) {
    if (key == SDLK_LEFT) {
        player->left = 1;
    }
    if (key == SDLK_RIGHT) {
        player->right = 1;
    }
    if (key == SDLK_UP) {
        player->up = 1;
    }
    if (key == SDLK_DOWN) {
        player->down = 1;
    }
}

void player_key_up(int key, struct Player* player) {
    if (key == SDLK_LEFT) {
        player->left = 0;
    }
    if (key == SDLK_RIGHT) {
        player->right = 0;
    }
    if (key == SDLK_UP) {
        player->up = 0;
    }
    if (key == SDLK_DOWN) {
        player->down = 0;
    }
}

void move_player(struct Player* player) {
    if(player->left) {
        player->position.x -= STEP;
    }
    if(player->right) {
        player->position.x += STEP;
    }
    if(player->up) {
        player->position.y -= STEP;
    }   
    if(player->down) {
        player->position.y += STEP;
    }
}

void resolve_keyboard(SDL_Event e, struct Player* player) {
    if (e.type == SDL_KEYDOWN) {
        player_key_down(e.key.keysym.sym, player);
    } 
    if (e.type == SDL_KEYUP) {
        player_key_up(e.key.keysym.sym, player);
    }
}
#include "objects.h"
#include "constants.h"
#include "maze_render.h"

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
    if (key == player->attack_key) {
        player->shoot = player->face;
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
    if (key == player->attack_key) {
        player->shoot = false;
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

void set_player_pos(struct Player* player, float x, float y) {
    player->position.x = x;
    player->position.y = y;
}

struct Bullet init_bullet(int x, int y, int face) {
    struct Bullet b;
    b.position.x = x;
    b.position.y = y;
    b.position.w = BULLET_WIDTH;
    b.position.h = BULLET_HEIGHT;
    b.face = face;
    return b;
}

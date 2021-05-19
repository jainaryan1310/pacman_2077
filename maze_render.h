#ifndef MAZE_RENDER_H
#define MAZE_RENDER_H

#include <SDL.h>
#include "objects.h"
#include "list.h"

void dfs(int x, int y);
void generate_maze();
int get_spawn_x(int pac_or_demon);
int get_spawn_y(int pac_or_demon);
void move_player(struct Player *player);
int get_coins();
SDL_Texture* get_map_texture(SDL_Renderer *renderer);
void move_bullets(struct node **bullets);
int check_if_player_dies(struct Player *player, struct node **bullets, int *killer);
void print_maze_values();

#endif

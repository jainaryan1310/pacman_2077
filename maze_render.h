#ifndef MAZE_RENDER_H
#define MAZE_RENDER_H

#include <SDL.h>
#include "objects.h"
#include "list.h"

void dfs(int x, int y);
void generate_maze();
void move_player(struct Player *player); 
SDL_Texture* get_map_texture(SDL_Renderer *renderer);
void move_bullets(struct node **bullets);
int check_if_player_dies(struct Player *player, struct node **bullets, int *killer);

#endif

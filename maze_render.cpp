#include <iostream>
#include <cstdlib>
#include "maze_render.h"
#include "constants.h"

using namespace std;

int height = 31;
int width = 45;
int coins = 0;

int maze[31][45];

void dfs(int x, int y) {
	maze[x][y] = 2;
	int neighbour = rand()%4;
	for (int i = 0; i < 4; i++) {
		if (neighbour == 0) {
			if (x-2 < 0 || maze[x-2][y] == 2) {
				neighbour += 1;
			}
			else {
				maze[x-1][y] = 0;
				dfs(x-2, y);
				neighbour += 1;
			}
		}
		else if (neighbour == 1) {
			if (y+2 >= width || maze[x][y+2] == 2) {
				neighbour += 1;
			}
			else {
				maze[x][y+1] = 0;
				dfs(x, y+2);
				neighbour += 1;
			}
		}
		else if (neighbour == 2) {
			if (x+2 >= height || maze[x+2][y] == 2) {
				neighbour += 1;
			}
			else {
				maze[x+1][y] = 0;
				dfs(x+2, y);
				neighbour += 1;
			}
		}
		else {
			if (y-2 < 0 || maze[x][y-2] == 2) {
				neighbour = 0;
			}
			else {
				maze[x][y-1] = 0;
				dfs(x, y-2);
				neighbour = 0;
			}
		}
	}
	return;
}

void generate_maze(int s) {
	for (int i = 0; i < height; i += 2) {
		for (int j = 0; j < width; j++) {
			maze[i][j] = 1;
		}
	}
	for (int i = 1; i < height; i += 2) {
		for (int j = 0; j < width; j += 2) {
			maze[i][j] = 1;
		}
	}
	srand(s);
	int x = rand()%height;
	int y = rand()%width;
	while (maze[x][y] == 1) {
		x = rand()%height;
		y = rand()%width;
	}
	dfs(x, y);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
            if (maze[i][j] == 0) {
                coins += 1;
            }
			if (maze[i][j] == 2) {
				maze[i][j] = 0;
                coins += 1;
			}
		}
	}
	return;
}

int get_coins() {
    return coins;
}

int sign(int a) {
    if (a < 0) {
        return -1;
    } else if (a > 0) {
        return 1;
    } else {
        return 0;
    }
}

void decrement_abs(int *a) {
    *a -= sign(*a);
}

int check_collisions(SDL_Rect *rect) {
    if (maze[2*rect->y/TILE_SIZE][2*rect->x/TILE_SIZE] == 1 ||
            maze[2*(rect->y + rect->h)/TILE_SIZE][2*rect->x/TILE_SIZE] == 1 ||
            maze[2*(rect->y)/TILE_SIZE][2*(rect->x + rect->w)/TILE_SIZE] == 1 ||
            maze[2*(rect->y + rect->h)/TILE_SIZE][2*(rect->x + rect->w)/TILE_SIZE] == 1 ||
            rect->x <= 0 || (2*(rect->x + rect->w) >= SCREEN_WIDTH)) {
        return true;
    } else {
        return false;
    }
}

bool eat_coin(struct Player *player) {
    struct SDL_Rect position = player->position;
    if (maze[2*position.y/TILE_SIZE][2*position.x/TILE_SIZE] != 2) {
        maze[2*position.y/TILE_SIZE][2*position.x/TILE_SIZE] = 2;
        coins -= 1;
        return true;
    }
    return false;
}


int move_and_check_collisions(SDL_Rect *position, int axis, int mov, bool pacman) {
    SDL_Rect temp = *position;

    if (axis == X_AXIS) {
        temp.x += sign(mov);
    }

    if (axis == Y_AXIS) {
        temp.y += -sign(mov);
    }

    if (check_collisions(&temp)) {
        return 0;
    } else {
        *position = temp;
        return 1;
    }
}
 
void move_bullets(struct node **bullets) {
    struct node *next = *bullets;
    struct Bullet *b = NULL;
    int i = 0;
    while (next != NULL) {
        b = (struct Bullet*) next->data;
        if (b->face == 0) {
            b->position.y -= PLAYER_SPEED;
        }
        if (b->face == 1) {
            b->position.x += PLAYER_SPEED;
        }
        if (b->face == 2) {
            b->position.y += PLAYER_SPEED;
        }
        if (b->face == 3) {
            b->position.x -= PLAYER_SPEED;
        }
        //b->position.x += PLAYER_SPEED * b->face * 1;
        next = next->next;
        if (check_collisions(&b->position)) {
            erase_element(bullets, i);
        } else {
            i++;
        }
    }
}

int check_if_player_dies(struct Player *player, struct node **bullets, int *killer) {
    struct node *next = *bullets;
    struct SDL_Rect b; 
    struct SDL_Rect p = player->position;
    int i = 0;
    while (next != NULL) {
        b = ((struct Bullet*) next->data)->position;
        if (p.x < (b.x + b.w) &&
                (p.x + p.w) > b.x &&
                p.y < (b.y + b.h) &&
                (p.y + p.h) > b.y) {
            *killer = ((struct Bullet*) next->data)->player_id;
            erase_element(bullets, i);
            return true;
        }
        next = next->next;
        i++;
    }
    return false;
}



void move_player(struct Player *player) {
    int x_movement = 0;
    int y_movement = 0;
    if (player->left) {
        x_movement -= PLAYER_SPEED;
        player->face = 3;
    }
    if (player->right) {
        x_movement += PLAYER_SPEED;
        player->face = 1;
    }
    if (player->up) {
        y_movement += PLAYER_SPEED;
        player->face = 0;
    }
    if (player->down) {
        y_movement -= PLAYER_SPEED;
        player->face = 2;
    }
    

    while (x_movement != 0 || y_movement != 0) {
        if (x_movement != 0 && move_and_check_collisions(&player->position, X_AXIS, x_movement, &player->pacman)) {
            decrement_abs(&x_movement);
        } else {
            x_movement = 0;
        }

        if (y_movement != 0 && move_and_check_collisions(&player->position, Y_AXIS, y_movement, &player->pacman)) {
            decrement_abs(&y_movement);
        } else {
            y_movement = 0;
        }
    }
}

void print_maze_values(){
    for (int i = 0; i<109; i++){
        for(int j = 0; j<145; j++){
            std::cout << maze[i][j] << " ";
        }
        std::cout << endl;
    }
}

SDL_Texture* get_map_texture(SDL_Renderer *renderer) {
    SDL_Surface *bmp = NULL;
    bmp = SDL_LoadBMP("coin.bmp");
    SDL_Surface *bitmap = NULL;
    SDL_Texture *map_texture;
    SDL_Rect rect;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;
    bitmap = SDL_LoadBMP("tile.bmp");
    SDL_Texture *tex = NULL;
    tex = SDL_CreateTextureFromSurface(renderer, bitmap);
    SDL_Texture *coin = NULL;
    coin = SDL_CreateTextureFromSurface(renderer, bmp);
    map_texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderTarget(renderer, map_texture);
    int i, j;
    for (i = 0; i < SCREEN_HEIGHT / TILE_SIZE; i++) {
        for (j = 0; j < SCREEN_WIDTH / TILE_SIZE; j++) {
            if (maze[i][j] == 1) {
                rect.x = TILE_SIZE * j;
                rect.y = TILE_SIZE * i;
                SDL_RenderCopy(renderer, tex, NULL, &rect);
            }else if (maze[i][j] == 0) {
                rect.x = TILE_SIZE * j;
                rect.y = TILE_SIZE * i;
                SDL_RenderCopy(renderer, coin, NULL, &rect);
            }
        }
    }
    SDL_SetRenderTarget(renderer, NULL);
    return map_texture;
}

int get_spawn_x(int pac_or_demon) {
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            if(maze[i][j] == 0){
                //std::cout << j << std::endl;
                return j*16;
            }
        }
    }
    return -1;
}

int get_spawn_y(int pac_or_demon) {
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            if(maze[i][j] == 0){
                //std::cout << i << std::endl;
                return i*16;
            }
        }
    }
    return -1;
}

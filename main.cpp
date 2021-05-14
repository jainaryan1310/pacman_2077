#include <stdio.h>
#include <SDL2/SDL.h>
#include "objects.h"



int main(){
	
	SDL_Window* main_window;
	main_window = SDL_CreateWindow ("pacman_2077", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
	if(main_window == NULL){
		printf("Game window could not be initiated : %s\n", SDL_GetError());
		return 1;
	}

    SDL_Renderer* main_renderer;
	main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if(main_renderer == NULL){
		SDL_DestroyWindow(main_window);
		printf("Game renderer could not be created : %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_Surface* loading_surface = NULL;
    loading_surface = SDL_LoadBMP("player.bmp");

    SDL_Texture* bg_texture = NULL;
    bg_texture = SDL_CreateTextureFromSurface(main_renderer, loading_surface);

    struct Player player;
    player.position.x = 10;
    player.position.y = 10;
    player.position.w = 10;
    player.position.h = 10;

    while(true){
    	
    	SDL_Event e;
    	
    	if(SDL_PollEvent(&e)){
    		if(e.type == SDL_QUIT){
    			break;
    		}
    		resolve_keyboard(e, &player);
    	}

    	move_player(&player);
    	SDL_RenderClear(main_renderer);
    	SDL_RenderCopy(main_renderer, bg_texture, NULL, &player.position);
    	SDL_RenderPresent(main_renderer);
    }

    SDL_DestroyTexture(bg_texture);
    SDL_DestroyRenderer(main_renderer);
    SDL_FreeSurface(loading_surface);
    SDL_DestroyWindow(main_window);
    SDL_Quit();
    return 0;
}
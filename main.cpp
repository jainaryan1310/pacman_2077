#include <stdio.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include "objects.h"
#include "client_udp.h"
#include "server_udp.h"
#include "network.h"
#include "maze_render.h"
#include "constants.h"
#include "font.h"
#include "menu.h"

using namespace std;

struct Player players[MAX_PLAYERS];
int number_of_players = 0;
int16_t my_id = -1;
int16_t bullets_client[256];
int bullets_number = 0;

SDL_Texture* load_texture(SDL_Renderer *renderer, char *file) {
    SDL_Surface *bitmap = NULL;
    SDL_Texture *texture = NULL;
    bitmap = SDL_LoadBMP(file);
    texture = SDL_CreateTextureFromSurface(renderer, bitmap);
    SDL_FreeSurface(bitmap);
    return texture;
}

void init_players() {
    int i;
    if(MAX_PLAYERS >= 1){
        players[0].position.x = get_spawn_x(1);
        players[0].position.y = get_spawn_y(1);
        players[0].position.w = PLAYER_WIDTH;
        players[0].position.h = PLAYER_HEIGHT;
        players[0].face = 1;
        players[0].shoot = false;
        players[0].reloading = false;
        players[0].kills = 0;
        players[0].deaths = 0;
        players[0].pacman = true;
        players[0].coins = 0;
    }
    for (i = 1; i < MAX_PLAYERS; i++) {
        players[i].position.x = get_spawn_x(0);
        players[i].position.y = get_spawn_y(0);
        players[i].position.w = PLAYER_WIDTH;
        players[i].position.h = PLAYER_HEIGHT;
        players[i].face = 1;
        players[i].shoot = false;
        players[i].reloading = false;
        players[i].kills = 0;
        players[i].deaths = 0;
        players[i].pacman = false;
        players[i].coins = 0;
    }
}

void receive_new_id(int id) {
    my_id = id;
    number_of_players = id;
    printf("my_id is now: %d\n", my_id);
}

void check_if_its_new_player(int id){
    if (id > number_of_players) {
        number_of_players = id;
        printf("new max player, now %d\n", number_of_players + 1);
    }
}


void* client_loop(void *arg) {
    int socket = *((int *) arg);
    int16_t tab[BUF_MAX];
    int length;
    int id, bullets_in_array;
    while (1) {
        length = client_listen(socket, tab);
        id = tab[0];
        if (id == -1) {
            receive_new_id(tab[1]);
        }
        if (id >= 0) {
            check_if_its_new_player(id);
            players[id].player_id = id;
            //cout << "player_id assigned: " << id << endl;
            players[id].position.x = tab[1];
            players[id].position.y = tab[2];
            players[id].kills = tab[3];
            players[id].deaths = tab[4];
        }
        if (id == -2) {
            bullets_in_array = (length - sizeof(int16_t)) / (sizeof(int16_t) * 2);
            memcpy(bullets_client, tab + 1, sizeof(int16_t) * 2 * bullets_in_array);
            bullets_number = bullets_in_array;
        }
        usleep(50);
    }
}

int main(int argc, char* argv[]){
    struct sockaddr_in server_addr, client_addr;
    int sock_server, sock_client;
    char *server_ip_addr = NULL;

    char menu = 's';
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Texture *pacman = NULL;
    SDL_Texture *demon = NULL;
    SDL_Texture *bullet = NULL;
    SDL_Texture *map = NULL;
    TTF_Init();
    TTF_Font *font;
    font = TTF_OpenFont("m5x7.ttf", 24);
    window = SDL_CreateWindow(
            "pacman_2077",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            720,
            496,
            0);

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        printf("Could not create renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    unsigned int n = stoi(argv[1]);
    generate_maze(n);
    init_players();
    
    pacman = load_texture(renderer, "pacman.bmp");
    demon = load_texture(renderer, "demon.bmp");
    //tex = load_texture(renderer, "player.bmp");
    bullet = load_texture(renderer, "bullet.bmp");
    int i;

    server_or_client(renderer, &menu, font);
    if (menu == 'c') {
        server_ip_addr = (char*) malloc(16 * sizeof(char));
        ask_for_ip(renderer, font, server_ip_addr);
    }
    pthread_t thread_id_server, thread_id_client, thread_id_server_send;
    server_addr = server_sock_addr(server_ip_addr);
    client_addr = client_sock_addr();
    if(menu == 's') {
        prepare_server(&sock_server, &server_addr);
        pthread_create(&thread_id_server, NULL, server_receive_loop, &sock_server);
        pthread_create(&thread_id_server_send, NULL, server_send_loop, &sock_server);
    }
    prepare_client(&sock_client, &client_addr);
    pthread_create(&thread_id_client, NULL, client_loop, &sock_client);

    while (my_id < 0) {
        send_to_server(sock_client, server_addr, my_id, 0);
        usleep(100);
    }

    SDL_Rect bullet_pos;
    bullet_pos.w = BULLET_HEIGHT;
    bullet_pos.h = BULLET_HEIGHT;

    SDL_Event e;

    while (1) {
        if (get_coins() <= 0) {
            int max = 0;
            for (int i = 0; i < MAX_PLAYERS; i++) {
                if (players[i].coins > max) {
                    max = i;
                }
            }
            cout << "Player " << i << " wins!" << endl;
            break;
        }
        if (players[my_id].deaths >= 5) {
            cout << "Player " << i << " loses!" << endl;
            break;
        }
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                break;
            }
            resolve_keyboard(e, &players[my_id]);
        }
        send_to_server(sock_client, server_addr, my_id, key_state_from_player(&players[my_id]));
        usleep(30);
        SDL_RenderClear(renderer);
        if (menu == 's'){
            map = get_map_texture_pacman(renderer);
        }else{
            map = get_map_texture_demon(renderer);
        }
        SDL_RenderCopy(renderer, map, NULL, NULL);
        SDL_RenderCopy(renderer, pacman, NULL, &players[0].position);
        for (i = 1; i <= number_of_players; i++) {
            SDL_RenderCopy(renderer, demon, NULL, &players[i].position);
        }

        disp_text(renderer, "kills", font, 400, 10);
        for (i = 0; i <= number_of_players; i++) {
            char kills[10] = {};
            sprintf(kills, "%d", players[i].kills);
            disp_text(renderer, kills, font, 400, 30 + i * 20);
        }

        disp_text(renderer, "deaths", font, 460, 10);
        for (i = 0; i <= number_of_players; i++) {
            char deaths[10] = {};
            sprintf(deaths, "%d", players[i].deaths);
            disp_text(renderer, deaths, font, 460, 30 + i * 20);
        }

        for (i = 0; i < bullets_number; i++) {
            bullet_pos.x = bullets_client[i*2];
            bullet_pos.y = bullets_client[i*2 + 1];
            SDL_RenderCopy(renderer, bullet, NULL, &bullet_pos);
        }
        SDL_DestroyTexture(map);
        SDL_RenderPresent(renderer);
    }

    close(sock_client);
    close(sock_server);
    pthread_cancel(thread_id_client);
    pthread_cancel(thread_id_server);
    pthread_cancel(thread_id_server_send);
    SDL_DestroyTexture(pacman);
    SDL_DestroyTexture(demon);
    SDL_DestroyTexture(bullet);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


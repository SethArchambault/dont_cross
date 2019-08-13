#include "program.h"
#include <stdlib.h>

#define _tile_w 60
#define _tile_half_w 30

#define lookRight(c) (map[y * _map_w + (x + 1)] == c)
#define lookDown(c)  (map[(y+1) * _map_w + x ]  == c)
#define lookUp(c)    (map[(y-1) * _map_w + x ]  == c)
#define lookLeft(c)  (map[y * _map_w + (x - 1)] == c)

Direction getDirectionOfRoad(Person *player, char * map) {
    int x = player->x;
    int y = player->y;
    int direction = player->direction;
    if (direction == R) { 
        if(lookRight('0'))  return R;
        if(lookDown('0'))   return D;
        if(lookUp('0'))     return U;
        if(lookLeft('0'))   return L;
    }
    if (direction == D) {
        if(lookDown('0'))   return D;
        if(lookRight('0'))  return R;
        if(lookLeft('0'))   return L;
        if(lookUp('0'))     return U;
    }
    if (direction == L){
        if(lookLeft('0'))   return L;
        if(lookUp('0'))     return U;
        if(lookDown('0'))   return D;
        if(lookRight('0'))  return R;
    }
    if (direction == U) {
        if(lookUp('0'))     return U;
        if(lookRight('0'))  return R;
        if(lookLeft('0'))   return L;
        if(lookDown('0'))   return D;
    }
    assert(0);
}

Direction getDirectionOfSidewalk(Person *player, char * map) {
    int x = player->x;
    int y = player->y;
    int direction = player->direction;
    if (direction == R) { 
        if(lookRight('s'))  return R;
        if(lookDown('s'))   return D;
        if(lookUp('s'))     return U;
        if(lookLeft('s'))   return L;
    }
    if (direction == D) {
        if(lookDown('s'))   return D;
        if(lookRight('s'))  return R;
        if(lookLeft('s'))   return L;
        if(lookUp('s'))     return U;
    }
    if (direction == L){
        if(lookLeft('s'))   return L;
        if(lookUp('s'))     return U;
        if(lookDown('s'))   return D;
        if(lookRight('s'))  return R;
    }
    if (direction == U) {
        if(lookUp('s'))     return U;
        if(lookRight('s'))  return R;
        if(lookLeft('s'))   return L;
        if(lookDown('s'))   return D;
    }
    printf("OH NO YOU WERE HIT BY A CAR!!!!!\n");
    exit(0);
}

void program(State *state) {
    void (*drawRect)();
	void (*saveMap)();
	void (*loadMap)();
	saveMap = state->saveMap;
	loadMap = state->loadMap;
    drawRect = state->drawRect;
	void * memory = state->memory;
	int *memory_i = &state->memory_i;
	
    if (!state->is_initialized) {
        state->is_initialized = 1;
		state->stage = 1;
        state->reload_map = 0;
        state->clock = 0;
		
		state->temporary_memory.memory = &state->memory[*memory_i];
		state->temporary_memory.limit = 1000;
		state->temporary_memory.high_water_mark = 0;
		*memory_i += state->temporary_memory.limit;
		state->map = (char *)&memory[*memory_i];
		*memory_i+= _map_w * _map_h * sizeof(char);
		// @safety: override map saving.
		//saveMap(&state->temporary_memory, 1,  state->map, _map_w, _map_h);

		loadMap(&state->temporary_memory, 1,  state->map);
        // strip symbols
        for (int i = 0; i < _map_size; ++i) {
            if (state->map[i] == '>') {
                state->map[i] = 's';
                state->player.x = i % _map_w;
                state->player.y = (int) i / _map_w;
                state->player.direction = R;
            }
            if (state->map[i] == '*') {
                state->map[i] = 's';
                state->goal.x = i % _map_w;
                state->goal.y = (int) i / _map_w;
            }
        }
    }
    if (state->reload_map) {
		loadMap(&state->temporary_memory, 1,  state->map);
    }

    Person * player = &state->player;
    Object*  goal = &state->goal;
    // movement
    ++state->clock;
    if (state->clock % 2) {
            if (player->x == goal->x && player->y == goal->y) {
                printf("!!!!!!!!!!!!!!!You won!!!!!!!!!!!!!!!!\n");
                exit(0);
            }
        if (state->input) {
            state->input = 0;
            player->direction = getDirectionOfRoad(player, state->map);
        }
        else {
            if (player->direction == R) {
                player->x += 1;
            }
            if (player->direction == L) {
                player->x -= 1;
            }
            if (player->direction == U) {
                player->y -= 1;
            }
            if (player->direction == D) {
                player->y += 1;
            }
            // don't do this if crossing
            player->direction = getDirectionOfSidewalk(player, state->map);
        }
        if (player->x < 0) player->x = 0;
        if (player->x >= _map_w) player->x = _map_w - 1;
        if (player->y < 0) player->y = 0;
        if (player->y >= _map_w) player->y = _map_h - 1;
    }

	Color white = (Color) { 255,255,255,255};
	Color grey = (Color) { 0,0, 50,255};
	Color blue = (Color) { 155,155,255,255};
	Color purple = (Color) { 244,91,255,255};
	Color gold = (Color) { 244,187,29,255};
	for (int y = 0; y < _map_h; ++y){
		for (int x = 0; x < _map_w; ++x){
			Color color = blue;
			assert(y * _map_w + x < _map_w * _map_h);
            char value = state->map[y * _map_w + x]; 
			if (value == 's') {
				color = white;
			}
            else if (value == 'w') {
				color = blue;
			}
            else if (value == '0') {
				color = grey;
			}
			drawRect(x * 62 , y * 62, _tile_w, _tile_w, color,  state->renderer);
		}// x
	}// y

    drawRect(goal->x * 62 , goal->y * 62, _tile_w, _tile_w, gold,  state->renderer);

    if (player->direction == R) {
        drawRect(player->x * 62 +_tile_half_w, player->y * 62, _tile_half_w, _tile_w, purple,  state->renderer);
    }
    if (player->direction == L) {
        drawRect(player->x * 62 , player->y * 62, _tile_half_w, _tile_w, purple,  state->renderer);
    }
    if (player->direction == U) {
        drawRect(player->x * 62 , player->y * 62, _tile_w, _tile_half_w, purple,  state->renderer);
    }
    if (player->direction == D) {
        drawRect(player->x * 62 , player->y * 62+_tile_half_w, _tile_w, _tile_half_w, purple,  state->renderer);
    }

}

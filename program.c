#include "program.h"
#include <stdlib.h>

#define _road '|'
#define _sidewalk '='
#define _footsteps ':'

#define _tile_w 60
#define _tile_half_w 30

#define lookRight(c) (x+1 < _map_w && map[y * _map_w + (x + 1)] == c)
#define lookDown(c)  (y+1 < _map_h && map[(y+1) * _map_w + x ]  == c)
#define lookUp(c)    (y-1 >= 0 && map[(y-1) * _map_w + x ]  == c)
#define lookLeft(c)  (x-1 >= 0 && map[y * _map_w + (x - 1)] == c)
#define lookPriorDirection(prior_direction)\
    if (prior_direction == R && lookRight(_sidewalk)) return R;\
    if (prior_direction == D && lookDown(_sidewalk))  return D;\
    if(prior_direction == U && lookUp(_sidewalk))     return U;\
    if(prior_direction == L && lookLeft(_sidewalk))   return L;

void reduceHealth(int *health) {
    printf("micro aggression!\n");
    health--;
    if (health == 0) {
        printf("You go home and sleep the rest of the day\n");
        exit(0);
    }
}

Direction getDirectionOfRoad(Person *player, char * map) {
    int x = player->x;
    int y = player->y;
    int direction = player->direction;
    if (direction == R) { 
        if(lookRight(_road))  return R;
        if(lookDown(_road))   return D;
        if(lookUp(_road))     return U;
        if(lookLeft(_road))   return L;
    }
    if (direction == D) {
        if(lookDown(_road))   return D;
        if(lookRight(_road))  return R;
        if(lookLeft(_road))   return L;
        if(lookUp(_road))     return U;
    }
    if (direction == L){
        if(lookLeft(_road))   return L;
        if(lookUp(_road))     return U;
        if(lookDown(_road))   return D;
        if(lookRight(_road))  return R;
    }
    if (direction == U) {
        if(lookUp(_road))     return U;
        if(lookRight(_road))  return R;
        if(lookLeft(_road))   return L;
        if(lookDown(_road))   return D;
    }
    assert(0);
}

Direction getDirectionOfSidewalk(Person *player, char * map, Direction prior_direction) {
    int x = player->x;
    int y = player->y;
    int direction = player->direction;
    if (direction == R) { 
        if(lookRight(_sidewalk) || lookRight(_footsteps))  return R;
        lookPriorDirection(prior_direction);
        if(lookDown(_sidewalk) || lookDown(_footsteps))   return D;
        if(lookUp(_sidewalk) || lookUp(_footsteps))     return U;
        if(lookLeft(_sidewalk) || lookLeft(_footsteps))   return L;
    }
    if (direction == D) {
        if(lookDown(_sidewalk) || lookDown(_footsteps))   return D;
        lookPriorDirection(prior_direction);
        if(lookRight(_sidewalk) || lookRight(_footsteps))  return R;
        if(lookLeft(_sidewalk) || lookLeft(_footsteps))   return L;
        if(lookUp(_sidewalk) || lookUp(_footsteps))     return U;
    }
    if (direction == L){
        if(lookLeft(_sidewalk) || lookLeft(_footsteps))   return L;
        lookPriorDirection(prior_direction);
        if(lookUp(_sidewalk) || lookUp(_footsteps))     return U;
        if(lookDown(_sidewalk) || lookDown(_footsteps))   return D;
        if(lookRight(_sidewalk) || lookRight(_footsteps))  return R;
    }
    if (direction == U) {
        if(lookUp(_sidewalk) || lookUp(_footsteps))     return U;
        lookPriorDirection(prior_direction);
        if(lookRight(_sidewalk) || lookRight(_footsteps))  return R;
        if(lookLeft(_sidewalk) || lookLeft(_footsteps))   return L;
        if(lookDown(_sidewalk) || lookDown(_footsteps))   return D;
    }
    printf("OH NO YOU WERE HIT BY A CAR!!!!!\n");
    exit(0);
}

Direction getDirectionOfCleanSidewalk(Person *player, char * map, Direction prior_direction) {
    int x = player->x;
    int y = player->y;
    int direction = player->direction;
    if (direction == R) { 
        if(lookRight(_sidewalk))  return R;
        lookPriorDirection(prior_direction);
        if(lookDown(_sidewalk))   return D;
        if(lookUp(_sidewalk))     return U;
        if(lookLeft(_sidewalk))   return L;
        return getDirectionOfRoad(player, map);
    }
    if (direction == D) {
        if(lookDown(_sidewalk))   return D;
        lookPriorDirection(prior_direction);
        if(lookRight(_sidewalk))  return R;
        if(lookLeft(_sidewalk))   return L;
        if(lookUp(_sidewalk))     return U;
        return getDirectionOfRoad(player, map);
    }
    if (direction == L){
        if(lookLeft(_sidewalk))   return L;
        lookPriorDirection(prior_direction);
        if(lookUp(_sidewalk))     return U;
        if(lookDown(_sidewalk))   return D;
        if(lookRight(_sidewalk))  return R;
        return getDirectionOfRoad(player, map);
    }
    if (direction == U) {
        if(lookUp(_sidewalk))     return U;
        lookPriorDirection(prior_direction);
        if(lookRight(_sidewalk))  return R;
        if(lookLeft(_sidewalk))   return L;
        if(lookDown(_sidewalk))   return D;
        return getDirectionOfRoad(player, map);
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
        state->walker_i = 0;
        state->health = 5;
		
		state->temporary_memory.memory = &state->memory[*memory_i];
		state->temporary_memory.limit = 1000;
		state->temporary_memory.high_water_mark = 0;
		*memory_i += state->temporary_memory.limit;
		state->map = (char *)&memory[*memory_i];
		*memory_i+= _map_w * _map_h * sizeof(char);
		state->walker = (Person *)&memory[*memory_i];
        *memory_i += sizeof(Person) * _walker_count;
        printf("memory_i %d\n", *memory_i);
		// @safety: override map saving.
		//saveMap(&state->temporary_memory, 1,  state->map, _map_w, _map_h);

		loadMap(&state->temporary_memory, 1,  state->map);
        // strip symbols
        int walker_i = 0;
        for (int i = 0; i < _map_size; ++i) {
            if (state->map[i] == 'p') {
                state->map[i] = _sidewalk;
                state->player.x = i % _map_w;
                state->player.y = (int) i / _map_w;
                state->player.direction = R;
            }
            if (state->map[i] == 'P') {
                state->map[i] = _sidewalk;
                state->player.x = i % _map_w;
                state->player.y = (int) i / _map_w;
                state->player.direction = L;
            }
            if (state->map[i] == 'w') {
                state->map[i] = _sidewalk;
                Person * walker = &state->walker[walker_i++];
                walker->x = i % _map_w;
                walker->y = (int) i / _map_w;
                walker->direction = R;
            }
            if (state->map[i] == 'W') {
                state->map[i] = _sidewalk;
                Person * walker = &state->walker[walker_i++];
                walker->x = i % _map_w;
                walker->y = (int) i / _map_w;
                walker->direction = L;
            }
            if (state->map[i] == '*') {
                state->map[i] = _sidewalk;
                state->goal.x = i % _map_w;
                state->goal.y = (int) i / _map_w;
            }
        }
        p(walker_i);
        state->walker_i = walker_i;
    }
    if (state->reload_map) {
		loadMap(&state->temporary_memory, 1,  state->map);
    }

    Person * player = &state->player;
    Object*  goal = &state->goal;
    // :movement
    ++state->clock;
    if (state->clock % 20 == 0) {
            if (player->x == goal->x && player->y == goal->y) {
                printf("!!!!!!!!!!!!!!!You won!!!!!!!!!!!!!!!!\n");
                exit(0);
            }
        // :input
        if (state->input) {
            state->prior_direction = state->player.direction;
            state->input = 0;
            // loop through all walkers - can any of them see me
            // and have an interrupted travel to me?
            int microaggression = 0;
            char * map = state->map;
            for (int i = 0; i < _walker_count; ++i) {
                Person * walker = &state->walker[i];
                if (walker->x != player->x && walker->y != walker->y) 
                    continue;
                Direction direction = walker->direction;
                int x = walker->x;
                int y = walker->y;
                if (player->y == y && direction == R) {
                    for(;map[y * _map_w + x] == _sidewalk; ++x) {
                        if (player->x == x) {
                            microaggression = 1;
                            break;
                        }
                    }
                }
                else if (player->y == y && direction == L) {
                    for(;map[y * _map_w + x] == _sidewalk; --x) {
                        if (player->x == x) {
                            microaggression = 1;
                            break;
                        }
                    }
                }
                else if (player->x == x && direction == U) {
                    for(;map[y * _map_w + x] == _sidewalk; --y) {
                        if (player->y == y) {
                            microaggression = 1;
                            break;
                        }
                    }
                }
                else if (player->x == x && direction == D) {
                    for(;map[y * _map_w + x] == _sidewalk; ++y) {
                        if (player->y == y) {
                            microaggression = 1;
                            break;
                        }
                    }
                }// else
            }// walker

            if (microaggression) {
                reduceHealth(&state->health);
            }
            player->direction = getDirectionOfRoad(player, state->map);
        }
        else {
            char * map_pos = &state->map[player->y * _map_w + player->x];
            if(*map_pos == _sidewalk) {
                *map_pos = _footsteps;
            }

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
            player->direction = getDirectionOfCleanSidewalk(player, state->map, state->prior_direction);
            
        }
        if (player->x < 0) player->x = 0;
        if (player->x >= _map_w) player->x = _map_w - 1;
        if (player->y < 0) player->y = 0;
        if (player->y >= _map_w) player->y = _map_h - 1;

        for (int i = 0; i < state->walker_i; ++i) {
            Person *walker = &state->walker[i];
            if (walker->direction == R) {
                walker->x += 1;
            }
            if (walker->direction == L) {
                walker->x -= 1;
            }
            if (walker->direction == U) {
                walker->y -= 1;
            }
            if (walker->direction == D) {
                walker->y += 1;
            }
            // don't do this if crossing
            // @Cleanup: This is a hack, walkers don't need a prior direction
            walker->direction = getDirectionOfSidewalk(walker, state->map, walker->direction);
        }
    }

	Color white = (Color) { 255,255,255,255};
	Color footsteps = (Color) { 0,100,255,255};
	Color grey = (Color) { 0,0, 50,255};
	Color blue = (Color) { 155,155,255,255};
	Color player_color = (Color) { 244,91,(int)((state->health /5.0f) * 255),255};
	Color walker_color = (Color) { 91,255,240,255};
	Color gold = (Color) { 244,187,29,255};
	for (int y = 0; y < _map_h; ++y){
		for (int x = 0; x < _map_w; ++x){
			Color color = blue;
			assert(y * _map_w + x < _map_w * _map_h);
            char value = state->map[y * _map_w + x]; 
			if (value == _sidewalk) {
				color = white;
			}
            else if (value == '.') {
				color = blue;
			}
            else if (value == _road) {
				color = grey;
			}
            else if (value == _footsteps) {
				color = footsteps;
			}
			drawRect(x * 62 , y * 62, _tile_w, _tile_w, color,  state->renderer);
		}// x
	}// y

    drawRect(goal->x * 62 , goal->y * 62, _tile_w, _tile_w, gold,  state->renderer);

    // :draw
    if (player->direction == R) {
        drawRect(player->x * 62 +_tile_half_w, player->y * 62, _tile_half_w, _tile_w, player_color,  state->renderer);
    }
    if (player->direction == L) {
        drawRect(player->x * 62 , player->y * 62, _tile_half_w, _tile_w, player_color,  state->renderer);
    }
    if (player->direction == U) {
        drawRect(player->x * 62 , player->y * 62, _tile_w, _tile_half_w, player_color,  state->renderer);
    }
    if (player->direction == D) {
        drawRect(player->x * 62 , player->y * 62+_tile_half_w, _tile_w, _tile_half_w, player_color,  state->renderer);
    }

    for (int i = 0; i < state->walker_i; ++i) {
        Person * walker = &state->walker[i];
        if (walker->direction == R) {
            drawRect(walker->x * 62 +_tile_half_w, walker->y * 62, _tile_half_w, _tile_w, walker_color,  state->renderer);
        }
        if (walker->direction == L) {
            drawRect(walker->x * 62 , walker->y * 62, _tile_half_w, _tile_w, walker_color,  state->renderer);
        }
        if (walker->direction == U) {
            drawRect(walker->x * 62 , walker->y * 62, _tile_w, _tile_half_w, walker_color,  state->renderer);
        }
        if (walker->direction == D) {
            drawRect(walker->x * 62 , walker->y * 62+_tile_half_w, _tile_w, _tile_half_w, walker_color,  state->renderer);
        }
    }
}
